#pragma once
#include "game_messages.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/security/key-chain.hpp>

class DataSync {
public:
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
    std::cout << server_prefix.toUri() << std::endl;
    std::cout << client_prefix.toUri() << std::endl;
    is_client = ic;
    id = identifier;

    face.setInterestFilter(ndn::Name(is_client ? client_prefix : server_prefix)
                               .append(std::to_string(id)),
                           std::bind(&DataSync::on_interest, this, _1, _2),
                           nullptr, onFailure);
  }

  void listen_for_data(int message_id, int datasender_id,
                       const ndn::DataCallback &on_data) {
    ndn::Name interest_name(is_client ? server_prefix : client_prefix);
    interest_name.append(std::to_string(datasender_id));
    interest_name.append(std::to_string(id));
    interest_name.append(std::to_string(message_id));

    ndn::Interest interest(interest_name);
    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true);
    face.expressInterest(
        interest,
        [&](const ndn::Interest &intrest, const ndn::Data &data) {
          listen_for_data(message_id, datasender_id, on_data);
          on_data(intrest, data);
        },
        [&](const ndn::Interest &, const ndn::lp::Nack &) {
          listen_for_data(message_id, datasender_id, on_data);
        },
        [&](const ndn::Interest &) {
          listen_for_data(message_id, datasender_id, on_data);
        });
  }

  void send_data(std::shared_ptr<NdnAgarioMessage> message,
                 bool only_send_latest, int receiver_id) {
    if (only_send_latest) {
      message_queue[message->id()][receiver_id].clear();
    }
    message_queue[message->id()][receiver_id].push_back(std::move(message));
  }

  void on_interest(const ndn::InterestFilter &, const ndn::Interest &interest) {
    int message_id = std::stoi(interest.getName().at(-1).toUri());
    int enquirer_index = std::stoi(interest.getName().at(-2).toUri());

    auto data = make_shared<ndn::Data>(interest.getName());
    data->setFreshnessPeriod(boost::chrono::milliseconds(0));
    Stream content = {true, {}};
    auto m = message_queue[message_id][enquirer_index].back();
    m->serialize(content);
    data->setContent(const_cast<uint8_t *>(content.data.data()),
                     content.data.size());

    keychain.sign(*data);
    face.put(*data);
    message_queue[message_id][enquirer_index].pop_back();
  }
};
