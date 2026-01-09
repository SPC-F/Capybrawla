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

void MultiplayerController::register_user(std::string uuid, std::string username) {
    for (uint16_t i = 0; i <= static_cast<uint16_t>(UserColor::GREEN); ++i) {
        auto color = static_cast<UserColor>(i);

        if (users_.find(color) == users_.end()) {
            User new_user = {uuid, username, color};
            users_.emplace(color, new_user);
            return;
        }
    }
}

void MultiplayerController::register_user(std::string uuid, std::string username, UserColor color) {
    User new_user = {uuid, username, color};
    users_.emplace(color, new_user);
}

void MultiplayerController::unregister_user(std::string uuid) {
    auto it = std::find_if(users_.begin(), users_.end(), [&](const auto& pair) {
            return pair.second.uuid == uuid;
        });

    if (it == users_.end()) return;

    auto shift_it = std::next(it);
    users_.erase(it);

    while (shift_it != users_.end()) {
        auto current = shift_it++;
        UserColor old_color = current->first;
        User value = current->second;

        users_.erase(current);

        UserColor new_color = static_cast<UserColor>(static_cast<uint16_t>(old_color) - 1);
        value.color = new_color;

        users_.emplace(new_color, value);
    }
}

void MultiplayerController::clear_users() {
    users_.clear();
}

std::map<UserColor, User> MultiplayerController::users() {
    return users_;
}