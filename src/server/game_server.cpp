#include "game_server.hpp"
#include "./../shared/game_messages.hpp"
#include "./../shared/game_settings.hpp"
#include "./../shared/serialization.hpp"
#include <csignal>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

namespace {
static volatile std::sig_atomic_t running = 1;
}

void signal_handler(int) { running = 0; }

GameServer::GameServer(int n_players) {
  flog = spdlog::get("flog");
  lobby_capacity = n_players;
  connected_players = 0;
  game_started = false;
  finished = 0;

  sync.init(SERVER_PREFIX, CLIENT_PREFIX, false, 0,
            std::bind(&GameServer::on_register_failed, this, _1, _2));

  // setup interest filters
  // join room
  sync.face.setInterestFilter(
      ndn::Name(SERVER_PREFIX).append("join"),
      std::bind(&GameServer::join_interest, this, _1, _2), nullptr,
      std::bind(&GameServer::on_register_failed, this, _1, _2));

}

void GameServer::on_register_failed(const ndn::Name &prefix,
                                    const std::string &reason) {
  std::cerr << "ERROR: Failed to register prefix '" << prefix
            << "' with the local forwarder (" << reason << ")" << std::endl;
  sync.face.shutdown();
  running = 0;
}

void GameServer::join_interest(const ndn::InterestFilter &,
                               const ndn::Interest &interest) {
  connected_players += 1;
  spdlog::info("new client ({}) is connected", connected_players);
  auto data = std::make_shared<ndn::Data>(interest.getName());
  Stream content = {true, {}};
  // send client_index to client
  Serialize::int_(content, &connected_players, 0, MAX_PLAYERS);
  data->setContent(content.data.data(), content.data.size());
  data->setFreshnessPeriod(boost::chrono::seconds(10));

  sync.keychain.sign(*data);
  sync.face.put(*data);
  register_listeners(connected_players);
  spdlog::info("registered event listeners for {}", connected_players);
}

void GameServer::client_disconnected(int client_index) {
  spdlog::warn("client {} disconnected", client_index);
  //  connected_players -= 1;
  //  int stats_size = state.players_stats.size();
  //  if (stats_size > client_index) {
  //    spdlog::warn("erased player {} stats", client_index);
  //    state.players_stats[client_index] = PlayerStats();
  //  }
  //  bool multicast = false;
  //  for (int i = 0; i < MAX_PLAYERS; i++) {
  //    if (i != client_index) {
  //      DeadPlayerMessage *message = (DeadPlayerMessage
  //      *)server.CreateMessage(
  //          i, (int)GameMessageType::DEAD_PLAYER);
  //      message->player_index = client_index;
  //      server.SendMessage(i, (int)GameChannel::RELIABLE, message);
  //      multicast = true;
  //    }
  //  }
  //  if (multicast)
  //    spdlog::debug("multicasted dead player message");
}

void GameServer::register_listeners(int client_id) {
  sync.listen_for_data((int)GameMessageType::NEW_PLAYER, client_id,
                       std::bind(&GameServer::process_message, this, _1, _2), 1);
  sync.listen_for_data((int)GameMessageType::ATE_PELLET, client_id,
                       std::bind(&GameServer::process_message, this, _1, _2), -1, 100);
  sync.listen_for_data((int)GameMessageType::PLAYER_UPDATE, client_id,
                       std::bind(&GameServer::process_message, this, _1, _2), -1, 100);
}

void GameServer::run() {
  flog->info("server running");
  std::signal(SIGINT, signal_handler);
  while (running != 0) {
    update();
  }
  stop();
}

void GameServer::update() {
  // stop server if all players finished the game
  if (game_started && connected_players == 0) {
    running = 0;
    return;
  }
  sync.face.processEvents(boost::chrono::milliseconds(16));
}

void GameServer::process_message(const ndn::Interest &interest,
                                 const ndn::Data &data) {
  int message_id = std::stoi(interest.getName().at(-1).toUri());
  int client_index = std::stoi(interest.getName().at(-4).toUri());
  spdlog::info("recevied data {} from {}", message_id, client_index);
  Stream stream = {false, {}};
  stream.read(const_cast<uint8_t *>(data.getContent().value()),
              data.getContent().value_size());
  switch (message_id) {
  case (int)GameMessageType::NEW_PLAYER: {
    NewPlayerMessage *npmessage = new NewPlayerMessage();
    npmessage->serialize(stream);
    process_newplayer_message(client_index, (NewPlayerMessage *)npmessage);
    delete npmessage;
  } break;
  case (int)GameMessageType::ATE_PELLET: {
    flog->info("received ate pellet message");
    AtePelletMessage *apmessage = new AtePelletMessage();
    apmessage->serialize(stream);
    process_atepellet_message(client_index, (AtePelletMessage *)apmessage);
    delete apmessage;
  } break;
  case (int)GameMessageType::PLAYER_UPDATE: {
    PlayerUpdateMessage *pumessage = new PlayerUpdateMessage();
    pumessage->serialize(stream);
    process_playerupdate_message(client_index,
                                 (PlayerUpdateMessage *)pumessage);
    delete pumessage;
  } break;
  default:
    spdlog::error("unexpected message");
    break;
  }
}

void GameServer::process_newplayer_message(int client_index,
                                           NewPlayerMessage *message) {
  flog->critical("newplayer: {}@{}",client_index, message->player_name);
  // store new player info
  std::string name(message->player_name);
  uint8_t r = message->r;
  uint8_t g = message->g;
  uint8_t b = message->b;
  flog->critical("Creating new slot for the player");
  state.players.emplace_back(name, r, g, b, 0);

  // send game info to all players if lobby's filled
  if (connected_players == lobby_capacity) {
    game_started = true;
  }
  if (game_started) {
    for (int client_id = 1; client_id <= connected_players; client_id++) {
      GameInfoMessage *reply = new GameInfoMessage();
      reply->player_index = client_id - 1;
      reply->players = std::move(state.get_players());
      reply->pellets = state.pellets;
      reply->viruses = state.viruses;
      sync.send_data(std::shared_ptr<GameInfoMessage>(reply), true, client_id);
      spdlog::info("sending game info");
    }
    flog->critical("sent gameinfo {}", state.players.size());
  }
}

void GameServer::process_atepellet_message(int, AtePelletMessage *message) {
  spdlog::info("process atepellet");
  auto reloc = state.relocate_pellet(message->pellet_id);
  auto new_x = std::get<0>(reloc);
  auto new_y = std::get<1>(reloc);
  // broadcast pellet relocation to all players
  PelletRelocMessage *reply = new PelletRelocMessage();
  reply->pellet_id = message->pellet_id;
  reply->pos_x = new_x;
  reply->pos_y = new_y;
  std::shared_ptr<PelletRelocMessage> sreply(reply);
  for (int index = 1; index <= connected_players; index++) {
    sync.send_data(std::shared_ptr<PelletRelocMessage>(sreply), true, index);
  }
}

void GameServer::process_playerupdate_message(int client_index,
                                              PlayerUpdateMessage *message) {
  spdlog::info("process player udpate");
  spdlog::debug("player update [{}] : cells {}, ejectiles {}", client_index,
                message->info.cells.size(), message->info.ejectiles.size());
  int n_stats = state.players_stats.size();
  if (n_stats <= client_index - 1) {
    state.players_stats.resize(client_index);
  }
  spdlog::debug("stats size : {}", state.players_stats.size());
  state.players_stats[client_index - 1] = std::move(message->info);

  // send snapshot
  if (game_started) {
    SnapshotMessage *reply = new SnapshotMessage();
    reply->seq_id = ++snapshot_id;
    reply->stats = state.players_stats;
    sync.send_data(std::shared_ptr<SnapshotMessage>(reply), true,
                   client_index);
    spdlog::debug("sent snapshot to {}", client_index);
  }
}

void GameServer::stop() {
  sync.face.shutdown();
  spdlog::critical("server is stopped.");
}
