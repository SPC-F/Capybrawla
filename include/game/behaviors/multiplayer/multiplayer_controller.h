#pragma once

#include <engine/network/multiplayer_service.h>
#include <engine/public/behavior.h>

#include <map>
#include <cstdint>

using connection_state_change_callback_t = std::function<void(ConnectionState, ConnectionState)>;

enum class UserColor : uint16_t {
  YELLOW = 0,
  BLUE,
  RED,
  GREEN
};

struct User {
  std::string uuid;
  UserColor color;
};

class MultiplayerController : public Behavior {
private:
  ConnectionState current_connection_state_ = ConnectionState::NONE;

  std::vector<connection_state_change_callback_t> connection_state_change_callbacks;

  std::map<UserColor, std::string> users_;

public:
    void on_update(float dt) override;

    void on_connection_state_change(connection_state_change_callback_t callback);

    void register_user(std::string uuid);
    void unregister_user(std::string uuid);

    std::map<UserColor, std::string> users();
};