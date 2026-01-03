#include <game/character/player_controller.h>
#include <game/character/gui/player_info_component.h>

#include <game/round/roundcontroller.h>
#include <game/prefabs/cloud_platform_object.h>
#include <game/prefabs/character/dying_capybara_object.h>
#include <game/network/message_types.h>

#include <engine/core/engine.h>
#include <engine/network/multiplayer_service.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/scene.h>
#include <engine/public/components/rigidbody_2d.h>

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

  GameObject& player_info_comp = PlayerInfoComponent::create_and_add(player.scene(), player);
  player_info_components.emplace(player.id(), std::ref(player_info_comp));

  realign_player_info_positions();
}

void RoundController::on_player_death(const PlayerObject &player) {
  std::optional<std::reference_wrapper<PlayerController>> controller_opt;

  for (auto behavior : player.get_components<BehaviorScript>()) {
    if (auto pc = dynamic_cast<PlayerController *>(&behavior.get().behavior())) {
      controller_opt = *pc;
    }
  }

  if (!controller_opt.has_value()) return;
  auto controller = &controller_opt->get();

  controller->lives(controller->lives() - 1);
  if (controller->lives() < 1) return;

  if (const auto& rigid_body_opt = player.get_component<Rigidbody2D>(); rigid_body_opt.has_value()) {
    spawn_dead_player(player);

    auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();
    if (multiplayer_service.get_peer_type() == PeerType::CLIENT) return;

    generate_new_spawn_position();
    if (multiplayer_service.get_peer_type() == PeerType::HOST) {
      if (const auto& network_id = player.get_component<NetworkIdentity>(); network_id.has_value()) {
        MsgUserRespawn body{};
        std::strncpy(body.uuid, network_id.value().get().uuid().c_str(), sizeof(body.uuid) - 1);
        body.x = spawn_position_.x;
        body.y = spawn_position_.y;
        body.z = spawn_position_.z;

        Message msg = serialize_message(body, CustomMessageTypes::USER_RESPAWN);
        multiplayer_service.send(msg);
      }
    }

    spawn_respawn_platform(player.transform().position(), spawn_position_);
   
    auto& rigid_body = rigid_body_opt->get();
    rigid_body.velocity({0, 0, 0});
    rigid_body.teleport(spawn_position_);
    
    controller->health(controller->max_health());
  }
}

void RoundController::spawn_dead_player(const PlayerObject &player) {
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

std::vector<Vector3> RoundController::spawn_positions() const {
  return spawn_positions_;
}

void RoundController::spawn_positions(std::vector<Vector3> positions) {
  spawn_positions_ = std::move(positions);
}

void RoundController::respawn_player(PlayerObject& player, Vector3 pos) {
  std::optional<std::reference_wrapper<PlayerController>> controller_opt;

  for (auto behavior : player.get_components<BehaviorScript>()) {
    if (auto pc = dynamic_cast<PlayerController *>(&behavior.get().behavior())) {
      controller_opt = *pc;
    }
  }

  if (!controller_opt.has_value()) return;
  auto controller = &controller_opt->get();

  if (const auto& rigid_body_opt = player.get_component<Rigidbody2D>(); rigid_body_opt.has_value()) {
    spawn_respawn_platform(player.transform().position(), pos);
   
    auto& rigid_body = rigid_body_opt->get();
    rigid_body.velocity({0, 0, 0});
    rigid_body.teleport(pos);
    
    controller->health(controller->max_health());
  }
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