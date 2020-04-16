#include "server_communicator.hpp"
#include <iostream>
#include <yojimbo/yojimbo.h>

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = {0};

ServerCommunicator::ServerCommunicator(const yojimbo::Address &server_address)
    : client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"),
             conn_config, adapter, 0.0) {
    sent_message = false;
    uint64_t clientId;
    yojimbo::random_bytes((uint8_t *)&clientId, 8);
    client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, server_address);
    running = true;
}

void ServerCommunicator::run() {
    float fixed_dt = 1.0f / 60.0f;
    while (running) {
        double current_time = yojimbo_time();
        if (time <= current_time) {
            update(fixed_dt);
            time += fixed_dt;
        } else {
            yojimbo_sleep(time - current_time);
        }
    }
}
void ServerCommunicator::update(float dt) {
    // update client
    client.AdvanceTime(client.GetTime() + dt);
    client.ReceivePackets();

    if (client.IsConnected()) {
        process_messages();

        // ... do connected stuff ...

        if (!sent_message) {
            NewPlayerMessage *message =
                (NewPlayerMessage *)client.CreateMessage(
                    (int)GameMessageType::NEW_PLAYER);
            message->r = 120;
            message->g = 120;
            message->b = 120;
            strcpy(message->player_name, "Dhruva");
            client.SendMessage((int)GameChannel::RELIABLE, message);
            sent_message = true;
        }
    }

    client.SendPackets();
}

void ServerCommunicator::process_messages() {
    for (int i = 0; i < conn_config.numChannels; i++) {
        yojimbo::Message *message = client.ReceiveMessage(i);
        while (message != NULL) {
            process_message(message);
            client.ReleaseMessage(message);
            message = client.ReceiveMessage(i);
        }
    }
}

void ServerCommunicator::process_message(yojimbo::Message *message) {
    switch (message->GetType()) {
    case (int)GameMessageType::NEW_PLAYER:
        process_newplayer_message((NewPlayerMessage *)message);
        break;
    default:
        break;
    }
}

void ServerCommunicator::process_newplayer_message(NewPlayerMessage *message) {
    std::cout << "New player : " << std::endl;
}
