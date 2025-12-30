#include <engine/public/components/rigidbody_2d.h>
#include <game/character/player_controller.h>
#include <game/round/roundcontroller.h>
#include <game/character/gui/player_info_component.h>

RoundController::RoundController(const Vector3 respawn_position): Behavior(), respawn_position_(respawn_position) {}

void RoundController::on_awake() {}
void RoundController::on_update(float dt) {}

void RoundController::add_player(PlayerObject &player, Scene &scene) {
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

  GameObject& player_info_comp = PlayerInfoComponent::create_and_add(scene, player);
  player_info_components.emplace(player.id(), std::ref(player_info_comp));

  realign_player_info_positions();
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
      auto& rigid_body = rigid_body_opt->get();
      rigid_body.teleport({650, 0, 0});
      rigid_body.velocity({0, 0, 0});
      controller->health(controller->max_health());
    }
  }
}

void RoundController::remove_player(PlayerObject &player) {
  std::erase_if(players,
                [&player](const std::reference_wrapper<PlayerObject> &p) {
                  return p.get().id() == player.id();
                });
  
  player_info_components.erase(player.id());
  realign_player_info_positions();
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

bool RoundController::is_player_registered(const PlayerObject& player) const {
  for (const auto& registered_player : players) {
    if (registered_player.get().id() == player.id()) {
      return true;
    }
  }
  return false;
}

void RoundController::realign_player_info_positions() {
  int num_players = player_info_components.size();
  if (num_players == 0) {
    return;
  }

  const float screen_width = 1920.0f;
  const float component_width = 350.0f;
  const float y_position = 950.0f;
  const float z_position = 0.0f;

  // Calculate leftover space after placing all components
  float leftover_space = screen_width - (num_players * component_width);
  
  // Distribute leftover space evenly across all gaps (left margin, between components, right margin)
  float num_gaps = num_players + 1.0f;
  float component_spacing = leftover_space / num_gaps;
  
  // Calculate starting x position with left margin
  float start_x = component_spacing;

  int index = 0;
  for (auto& [player_id, player_info_ptr] : player_info_components) {
    // Position each component with spacing
    float x_position = start_x + (index * (component_width + component_spacing));
    player_info_ptr.get().transform().position({x_position, y_position, z_position});
    index++;
  }
}