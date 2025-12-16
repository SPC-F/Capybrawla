#include "engine/public/components/rigidbody_2d.h"

#include <engine/public/scene.h>
#include <game/character/player_controller.h>

#include <game/round/roundcontroller.h>

RoundController::RoundController(const Vector3 respawn_position): Behavior(), respawn_position_(respawn_position) {}

void RoundController::on_awake() {}
void RoundController::on_update(float dt) {}

void RoundController::add_player(PlayerObject &player) {
  for (auto behavior : player.get_components<BehaviorScript>()) {
    if (auto pc = dynamic_cast<PlayerControllerBehavior *>(&behavior.get().behavior())) {
      pc->on_health_changed([&](int old_health, int new_health) {
        if (pc->is_alive()) {
          return;
        }
        on_player_death(player);
      });
    }
  }

  players.push_back(player);
}

void RoundController::on_player_death(const PlayerObject &player) {
  for (auto behavior : player.get_components<BehaviorScript>()) {
    const auto pc = dynamic_cast<PlayerControllerBehavior *>(&behavior.get().behavior());
    if (!pc) {
      continue;
    }

    pc->lives(pc->lives() - 1);

    if (pc->lives() < 1) {
      pc->disable();
      return;
    }

    if (const auto& rigid_body_opt = player.get_component<Rigidbody2D>(); rigid_body_opt.has_value()) {
      auto& rigid_body = rigid_body_opt->get();
      rigid_body.teleport({650, 0, 0});
      rigid_body.velocity({0, 0, 0});
    }
  }
}

void RoundController::remove_player(PlayerObject &player) {
  std::erase_if(players,
                [&player](const std::reference_wrapper<PlayerObject> &p) {
                  return p.get().id() == player.id();
                });
}

void RoundController::round_end() const {
  for (auto &callback : round_end_callbacks) {
    callback();
  }
}

void RoundController::on_round_end(const round_end_callback_t &callback) {
  round_end_callbacks.push_back(callback);
}

Vector3 RoundController::respawn_position() const {
  return respawn_position_;
}
void RoundController::respawn_position(Vector3 position) {
  respawn_position_ = position;
}