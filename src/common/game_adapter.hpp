#pragma once
#include "IServerConnection.hpp"
#include "game_message_factory.hpp"
#include <yojimbo/yojimbo.h>

// the adapter
class GameAdapter : public yojimbo::Adapter {
public:
  explicit GameAdapter(IServerConnection *server = NULL) : m_server(server) {}

  yojimbo::MessageFactory *
  CreateMessageFactory(yojimbo::Allocator &allocator) override {
    return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
  }

  void OnServerClientConnected(int client_index) override {
    if (m_server != NULL) {
      m_server->client_connected(client_index);
    }
  }

  void OnServerClientDisconnected(int client_index) override {
    if (m_server != NULL) {
      m_server->client_disconnected(client_index);
    }
  }

private:
  IServerConnection *m_server;
};
