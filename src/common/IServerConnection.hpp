#pragma once
class IServerConnection {
public:
  virtual void client_connected(int clientIndex) = 0;
  virtual void client_disconnected(int clientIndex) = 0;
};
