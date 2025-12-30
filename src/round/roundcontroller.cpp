#include <game/character/player_controller.h>

#include <game/round/roundcontroller.h>
#include <game/prefabs/cloud_platform_object.h>

#include <engine/public/scene.h>
#include <engine/public/components/rigidbody_2d.h>
#include <game/prefabs/character/dying_capybara_object.h>

constexpr float RESPAWN_PLATFORM_X_OFFSET = 0.0f;
constexpr float RESPAWN_PLATFORM_Y_OFFSET = 80.0f;

RoundController::RoundController(std::vector<Vector3> spawn_positions): Behavior(), spawn_positions_(std::move(spawn_positions)) {}

void RoundController::on_awake() {}
void RoundController::on_update(float dt) {}

void RoundController::generate_new_spawn_position() {
  std::uniform_int_distribution<size_t> distr(0, spawn_positions_.size() - 1);
  size_t spawn_index = distr(gen_);

  if (spawn_positions_.size() > 1) {
    while (spawn_index == last_spawn_index_) {
      spawn_index = distr(gen_);
    }
  }
  last_spawn_index_ = spawn_index;

  spawn_position_ = spawn_positions_[spawn_index];
}

void RoundController::add_player(PlayerObject &player) {
  for (auto behavior : player.get_components<BehaviorScript>()) {
    if (auto pc = dynamic_cast<PlayerController *>(&behavior.get().behavior())) {

      on_player_health_changed_subscriptions.try_emplace(player.id(), std::move(pc->on_health_changed([&, pc](int old_health, int new_health) {
        if (pc->is_alive()) {
          return;
        }
        on_player_death(player);
      })));
    }
  }

  players.push_back(player);
}

void RoundController::on_player_death(const PlayerObject &player) {
  for (auto behavior : player.get_components<BehaviorScript>()) {
    const auto controller = dynamic_cast<PlayerController *>(&behavior.get().behavior());
    if (!controller) {
      continue;
    }

    controller->lives(controller->lives() - 1);

    if (controller->lives() < 1) {
      return;
    }

    if (const auto& rigid_body_opt = player.get_component<Rigidbody2D>(); rigid_body_opt.has_value()) {
      generate_new_spawn_position();
      
      spawn_dead_player(player, spawn_position_);
      spawn_respawn_platform(player.transform().position(), spawn_position_);

      auto& rigid_body = rigid_body_opt->get();
      rigid_body.velocity({0, 0, 0});
      rigid_body.teleport(spawn_position_);
      
      controller->health(controller->max_health());
    }
  }
}

void RoundController::spawn_dead_player(const PlayerObject &player, Vector3 spawn_position) {
  game_object().scene().add_game_object<DyingCapybaraObject>(game_object().scene(), player.transform().position(), 2.0f);  
}

void RoundController::spawn_respawn_platform(const Vector3 &position, Vector3 spawn_position) {
  Vector3 platform_position = spawn_position + Vector3{RESPAWN_PLATFORM_X_OFFSET, RESPAWN_PLATFORM_Y_OFFSET, 0};
  game_object().scene().add_game_object<CloudPlatformObject>(game_object().scene(), platform_position);
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

std::vector<Vector3> RoundController::spawn_positions() const {
  return spawn_positions_;
}

void RoundController::spawn_positions(std::vector<Vector3> positions) {
  spawn_positions_ = std::move(positions);
}