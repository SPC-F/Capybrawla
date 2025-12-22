#pragma once

#include <engine/network/multiplayer_service.h>
#include <engine/public/behavior.h>

using connection_state_change_callback_t = std::function<void(ConnectionState, ConnectionState)>;

class MultiplayerController : public Behavior {
private:
  ConnectionState current_connection_state_ = ConnectionState::NONE;

  std::vector<connection_state_change_callback_t> connection_state_change_callbacks;

public:
    void on_update(float dt) override;

    void on_connection_state_change(connection_state_change_callback_t callback);
};