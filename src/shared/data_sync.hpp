#pragma once
#include "game_messages.hpp"
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <spdlog/spdlog.h>

class DataSync {
public:
  DataSync() : face("127.0.0.1") {}
  ndn::Name server_prefix;
  ndn::Name client_prefix;
  bool is_client;
  int id;
  ndn::Face face;
  ndn::KeyChain keychain;

  std::map<int, std::map<int, std::vector<std::shared_ptr<NdnAgarioMessage>>>>
      message_queue;

  void init(const char *sp, const char *cp, bool ic, int identifier,
            const ndn::RegisterPrefixFailureCallback &onFailure) {
    server_prefix = ndn::Name(sp);
    client_prefix = ndn::Name(cp);
    is_client = ic;
    id = identifier;

    auto interest_prefix = ndn::Name(is_client ? client_prefix : server_prefix)
                               .append(std::to_string(id));
    face.setInterestFilter(interest_prefix,
                           std::bind(&DataSync::on_interest, this, _1, _2),
                           nullptr, onFailure);
    spdlog::info("data sync enabled, listening on {}", interest_prefix.toUri());
  }

  void listen_for_data(int message_id, int datasender_id,
                       const ndn::DataCallback &on_data, int nmessages = -1,
                       int timeout = 4000) {
    ndn::Name interest_name(is_client ? server_prefix : client_prefix);
    interest_name.append(std::to_string(datasender_id));
    interest_name.appendVersion();
    interest_name.append(std::to_string(id));
    interest_name.append(std::to_string(message_id));

    ndn::Interest interest(interest_name);
    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(boost::chrono::milliseconds(timeout));
    face.expressInterest(
        interest,
        [message_id, datasender_id, on_data, nmessages, timeout,
         this](const ndn::Interest &intrest, const ndn::Data &data) {
          if (nmessages - 1 != 0) {
            listen_for_data(message_id, datasender_id, on_data,
                            (nmessages == -1) ? -1 : (nmessages - 1),
                            std::abs(timeout - 1));
          }
          on_data(intrest, data);
        },
        [message_id, datasender_id, on_data, nmessages, timeout,
         this](const ndn::Interest &, const ndn::lp::Nack &) {
          listen_for_data(message_id, datasender_id, on_data, nmessages,
                          timeout + 1);
        },
        [message_id, datasender_id, on_data, nmessages, timeout,
         this](const ndn::Interest &) {
          listen_for_data(message_id, datasender_id, on_data, nmessages,
                          timeout + 1);
        });
  }

  void send_data(std::shared_ptr<NdnAgarioMessage> message,
                 bool only_send_latest, int receiver_id) {
    int mid = message->id();
    if (only_send_latest) {
      message_queue[mid][receiver_id].clear();
    }
    message_queue[mid][receiver_id].push_back(std::move(message));
    spdlog::info("save data in store at [{}][{}], qsize: {}", mid, receiver_id,
                 message_queue[mid][receiver_id].size());
  }

  void on_interest(const ndn::InterestFilter &, const ndn::Interest &interest) {
    int message_id = std::stoi(interest.getName().at(-1).toUri());
    int enquirer_index = std::stoi(interest.getName().at(-2).toUri());
    spdlog::info("received an interest for {} from {}", message_id,
                 enquirer_index);

    if (message_queue[message_id][enquirer_index].size() > 0) {
      auto data = std::make_shared<ndn::Data>(interest.getName());
      Stream content = {true, {}};
      auto m = message_queue[message_id][enquirer_index].back();
      m->serialize(content);
      data->setContent(const_cast<uint8_t *>(content.data.data()),
                       content.data.size());
      data->setFreshnessPeriod(boost::chrono::seconds(10));

      keychain.sign(*data);
      face.put(*data);
      message_queue[message_id][enquirer_index].pop_back();
      spdlog::info("sent data!");
    } else {
      spdlog::info("ignoring interest... no data to send!");
    }
  }
};
