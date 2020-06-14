#include "network_client.hpp"
#include "./../shared/serialization.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

NetworkClient::NetworkClient(World* w) : world(w){
  running = true;
  update_id = 0;
  flog = spdlog::get("flog");
  last_snapshot_id = 0;
  //connected = false;
}

void NetworkClient::connect_to_server(std::string player_name) {
  ndn::Name interestName(SERVER_PREFIX);

  ndn::Interest interest(interestName.append("join").appendVersion());
  interest.setCanBePrefix(false);
  interest.setMustBeFresh(true);

  sync.face.expressInterest(
      interest,
      [this, player_name](const ndn::Interest &i, const ndn::Data &data) {
        std::cout << "received interest" << std::endl;
        join_room(i, data, player_name);
      },
      [this, player_name](const ndn::Interest &, const ndn::lp::Nack &) {
      std::cout << "NAC" << std::endl;
        connect_to_server(player_name);
      },
      [this, player_name](const ndn::Interest &) {
      std::cout << "timeout" << std::endl;
        connect_to_server(player_name);
      });
}

void NetworkClient::update() {
  // update client
  sync.face.processEvents(boost::chrono::milliseconds(10));
  send_playerupdate();
}

void NetworkClient::process_message(const ndn::Interest &interest,
                                    const ndn::Data &data) {

  int message_id = std::stoi(interest.getName().at(-1).toUri());
  spdlog::info("received message {} from server", message_id);
  Stream stream = {false, {}};
  stream.read(const_cast<uint8_t *>(data.getContent().value()),
              data.getContent().value_size());
  switch (message_id) {
  case (int)GameMessageType::NEW_PLAYER: {
    NewPlayerMessage *npmessage = new NewPlayerMessage();
    npmessage->serialize(stream);
    process_newplayer_message((NewPlayerMessage *)npmessage);
    delete npmessage;
  } break;
  case (int)GameMessageType::GAME_INFO: {
    GameInfoMessage *gimessage = new GameInfoMessage();
    gimessage->serialize(stream);
    process_gameinfo_message((GameInfoMessage *)gimessage);
    delete gimessage;
  } break;
  case (int)GameMessageType::PELLET_RELOC: {
    PelletRelocMessage *prmessage = new PelletRelocMessage();
    prmessage->serialize(stream);
    process_pelletreloc_message((PelletRelocMessage *)prmessage);
    delete prmessage;
  } break;
  case (int)GameMessageType::SNAPSHOT: {
    SnapshotMessage *smessage = new SnapshotMessage();
    smessage->serialize(stream);
    process_snapshot_message((SnapshotMessage *)smessage);
    delete smessage;
  } break;
  case (int)GameMessageType::DEAD_PLAYER: {
    DeadPlayerMessage *dpmessage = new DeadPlayerMessage();
    dpmessage->serialize(stream);
    process_deadplayer_message((DeadPlayerMessage *)dpmessage);
    delete dpmessage;
  } break;
  case (int)GameMessageType::GAME_OVER: {
    GameOverMessage *gomessage = new GameOverMessage();
    gomessage->serialize(stream);
    process_gameover_message((GameOverMessage *)gomessage);
    delete gomessage;
  } break;
  default:
    std::cout << "unexpected message\n" << std::endl;
    break;
  }
}

void NetworkClient::process_newplayer_message(NewPlayerMessage *message) {
  spdlog::info("received new player message");
  int p_index = message->player_index;
  int p_info_size = (*world).players_info.size();
  if (p_info_size <= p_index) {
    spdlog::debug("inserting opponent in new slot {}", p_index);
    (*world).players_info.resize(p_index + 1);
  } else {
    spdlog::debug("using pre existing slot for opponent {}", p_index);
  }
  (*world).players_info[p_index] =
      std::make_tuple(true, std::string(message->player_name), message->r,
                      message->g, message->b);
}

void NetworkClient::process_gameinfo_message(GameInfoMessage *message) {
  spdlog::debug("received game info message");
  (*world).my_index = message->player_index;
  auto players = message->players;
  auto pellets = message->pellets;
  auto viruses = message->viruses;
  flog->critical("players size: {}", players.size());

  for (auto &player : players) {
    (*world).add_player(std::move(player));
  }

  for (auto &pellet : pellets) {
    (*world).create_pellet(std::move(pellet));
  }

  for (auto &virus : viruses) {
    (*world).create_virus(std::move(virus));
  }
}

void NetworkClient::process_pelletreloc_message(PelletRelocMessage *message) {
  spdlog::debug("received pellet reloc message");
  int pellet_id = message->pellet_id;
  int x = message->pos_x;
  int y = message->pos_y;

  (*world).relocate_pellet(pellet_id, x, y);
}

void NetworkClient::process_gameover_message(GameOverMessage *message) {
  spdlog::warn("received game over message");
  spdlog::info("Finished in {} place", message->rank);
  (*world).running = false;
}

void NetworkClient::join_room(const ndn::Interest &, const ndn::Data &data,
                              std::string player_name) {
  spdlog::info("receive join room data!");
  int client_id;
  Stream stream = {false, {}};
  //connected = true;
  stream.read(const_cast<uint8_t *>(data.getContent().value()),
              data.getContent().value_size());
  Serialize::int_(stream, &client_id, 0, MAX_PLAYERS);
  flog->critical("uid: {}", client_id);

  sync.init(SERVER_PREFIX, CLIENT_PREFIX, true, client_id,
            std::bind(&NetworkClient::on_register_failed, this, _1, _2));

  // setup listeners
  sync.listen_for_data((int)GameMessageType::NEW_PLAYER, 0,
                       [this](const ndn::Interest &interest, const ndn::Data &d) {
                         process_message(interest, d);
                       });
  sync.listen_for_data((int)GameMessageType::GAME_INFO, 0,
                       [this](const ndn::Interest &interest, const ndn::Data &d) {
                         process_message(interest, d);
                       }, 1, 100);
  sync.listen_for_data((int)GameMessageType::PELLET_RELOC, 0,
                       [this](const ndn::Interest &interest, const ndn::Data &d) {
                         process_message(interest, d);
                       }, -1, 50);
  sync.listen_for_data((int)GameMessageType::SNAPSHOT, 0,
                       [this](const ndn::Interest &interest, const ndn::Data &d) {
                         process_message(interest, d);
                       }, -1, 50);
  sync.listen_for_data((int)GameMessageType::DEAD_PLAYER, 0,
                       [this](const ndn::Interest &interest, const ndn::Data &d) {
                         process_message(interest, d);
                       });
  sync.listen_for_data((int)GameMessageType::GAME_OVER, 0,
                       [this](const ndn::Interest &interest, const ndn::Data &d) {
                         process_message(interest, d);
                       }, 1);

  NewPlayerMessage *message = new NewPlayerMessage();
  message->r = ((*world).agar)->r;
  message->g = ((*world).agar)->g;
  message->b = ((*world).agar)->b;
  message->player_name = player_name;
  sync.send_data(std::shared_ptr<NewPlayerMessage>(message), true, 0);
  spdlog::info("sending new player message..");
}

void NetworkClient::send_atepellet_message(int id) {
  spdlog::debug("sent ate pellet message");
  AtePelletMessage *message = new AtePelletMessage();
  message->pellet_id = id;
  sync.send_data(std::shared_ptr<AtePelletMessage>(message), true, 0);
}

void NetworkClient::send_playerupdate() {
  spdlog::debug("sent player update message");
  PlayerStats info = ((*world).agar)->get_player_stats();
  PlayerUpdateMessage *message = new PlayerUpdateMessage();
  message->seq_id = ++update_id;
  message->info = std::move(info);
  sync.send_data(std::shared_ptr<PlayerUpdateMessage>(message), true, 0);
}

void NetworkClient::process_snapshot_message(SnapshotMessage *message) {
  flog->critical("received snapshot message");
  (*world).players_stats = std::move(message->stats);
  spdlog::debug("received snapshot message");
}

void NetworkClient::process_deadplayer_message(DeadPlayerMessage *message) {
  spdlog::warn("received dead player message for {}", message->player_index);
  int n_players = (*world).players_info.size();
  if (n_players > message->player_index) {
    std::get<0>((*world).players_info[message->player_index]) = false;
    flog->warn("deactivated player {}",
               std::get<1>((*world).players_info[message->player_index]));
  }
}

void NetworkClient::close_connection() { sync.face.shutdown(); }

void NetworkClient::on_register_failed(const ndn::Name &prefix,
                                       const std::string &reason) {
  std::cerr << "ERROR: Failed to register prefix '" << prefix
            << "' with the local forwarder (" << reason << ")" << std::endl;
  sync.face.shutdown();
  running = 0;
}
