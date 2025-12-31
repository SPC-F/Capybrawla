#include <engine/core/engine.h>

#include <game/behaviors/multiplayer/multiplayer_controller.h>
#include <iostream>

void MultiplayerController::on_update(float dt) {
    Engine& engine = Engine::instance();
    auto& multiplayer_service = engine.services->get_service<MultiplayerService>().get();

    if (multiplayer_service.get_connection_state() != current_connection_state_) {

        switch (multiplayer_service.get_connection_state()) {
            case ConnectionState::CONNECTED: std::cout << "Connected" << std::endl; break;
            case ConnectionState::CONNECTING: std::cout << "Cnnecting" << std::endl; break;
            case ConnectionState::DISCONNECTED: std::cout << "Disconnected" << std::endl; break;
            case ConnectionState::DISCONNECTING: std::cout << "Disconnecting" << std::endl; break;
            case ConnectionState::NONE: std::cout << "None" << std::endl; break;
            case ConnectionState::RECONNECTING: std::cout << "Reconnecting" << std::endl; break;
        }

        ConnectionState new_connection_state = multiplayer_service.get_connection_state();

        for (connection_state_change_callback_t callback : connection_state_change_callbacks) {
            callback(current_connection_state_, new_connection_state);
        }

        current_connection_state_ = new_connection_state;
    }
}

void MultiplayerController::on_connection_state_change(connection_state_change_callback_t callback) {
    connection_state_change_callbacks.push_back(callback);
}