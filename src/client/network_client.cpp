#include "network_client.hpp"
#include <iostream>
#include <yojimbo/yojimbo.h>

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = {0};

NetworkClient::NetworkClient(const yojimbo::Address &server_address)
    : client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"),
             conn_config, adapter, 0.0) {
    sent_info = false;
    uint64_t clientId;
    yojimbo::random_bytes((uint8_t *)&clientId, 8);
    client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, server_address);
    running = true;
}

void NetworkClient::run() {
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
void NetworkClient::update(float dt) {
    // update client
    client.AdvanceTime(client.GetTime() + dt);
    client.ReceivePackets();

    if (client.IsConnected()) {
        process_messages();

        // ... do connected stuff ...

        if (!sent_info) {
            NewPlayerMessage *message =
                (NewPlayerMessage *)client.CreateMessage(
                    (int)GameMessageType::NEW_PLAYER);
            message->r = 120;
            message->g = 120;
            message->b = 120;
            strcpy(message->player_name, "Dhruva");
            client.SendMessage((int)GameChannel::RELIABLE, message);
            sent_info = true;
        }
    }

    client.SendPackets();
}

void NetworkClient::process_messages() {
    for (int i = 0; i < conn_config.numChannels; i++) {
        yojimbo::Message *message = client.ReceiveMessage(i);
        while (message != NULL) {
            process_message(message);
            client.ReleaseMessage(message);
            message = client.ReceiveMessage(i);
        }
    }
}

void NetworkClient::process_message(yojimbo::Message *message) {
    switch (message->GetType()) {
    case (int)GameMessageType::PELLETS_INFO:
        process_pelletsinfo_message((PelletsInfoMessage *)message);
        break;
    case (int)GameMessageType::PELLET_RELOC:
        process_pelletreloc_message((PelletRelocMessage *)message);
        break;
    case (int)GameMessageType::GAME_OVER:
        process_gameover_message((GameOverMessage *)message);
        break;
    default:
        std::cout << "unexpected message\n" << std::endl;
        break;
    }
}

void NetworkClient::process_pelletsinfo_message(PelletsInfoMessage *message) {
    std::cout << "Pellets info : " << std::endl;
}

void NetworkClient::process_pelletreloc_message(PelletRelocMessage *message) {
    std::cout << "Pellet reloc : " << std::endl;
}

void NetworkClient::process_gameover_message(GameOverMessage *message) {
    std::cout << "Gameover : " << std::endl;
}
