
#include <game/character/player_movement_behavior.h>
#include <game/character/player_object.h>
#include <game/character/player_controller.h>
#include <game/character/player_outofbounds_behavior.h>
#include <game/character/player_weapon_controller.h>
#include <game/prefabs/weapons/weapon_bat_player_object.h>
#include <game/prefabs/weapons/weapon_axe_player_object.h>
#include <game/prefabs/weapons/weapon_boxing_gloves_player_object.h>
#include <game/prefabs/weapons/weapon_sword_player_object.h>

#include <engine/core/engine.h>
#include <engine/public/components/animator.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/network_identity.h>
#include <engine/network/multiplayer_service.h>

PlayerObject::PlayerObject(Scene &scene, const Vector3 initial_pos, bool is_local_player)
    : GameObject(scene), user_name_{"PLACEHOLDER"} {
  this->name("PlayerObject");
  this->tag("Player");
  this->transform().position(initial_pos);

  constexpr float scale_factor = 2.0f;
  this->transform().scale({scale_factor, scale_factor, 1.0f});

  // Visuals
  this->add_component<Sprite>("capybara_default_idle", Color(), 0, 0, 0, 0);
  this->add_component<Animator>("capybara_default_walk_anim", 95);

  // Physics
  constexpr float default_height = 28 * scale_factor;
  constexpr float default_x_offset = 6 * scale_factor;
  const Point default_offset = {default_x_offset, 4 * scale_factor};

  this->add_component<Rigidbody2D>(BodyType2D::Type::Dynamic, 35.0f, true, 3.0f);
  this->add_component<BoxCollider2D>(0.6f, .0f, 20 * scale_factor,
                                     default_height, default_offset);

  this->add_component<BehaviorScript>(std::make_unique<PlayerController>(100, 100));
  this->add_component<BehaviorScript>(std::make_unique<PlayerMovementBehavior>(
      default_height, default_height / 2.0f, default_offset,
      Point{default_x_offset, default_height / 3.2f * scale_factor}));

  // Default weapons
  if (is_local_player) {
    auto& weapon = scene.add_game_object<WeaponBoxingGlovesPlayerObject>(scene, *this);
    this->add_component<BehaviorScript>(std::make_unique<PlayerWeaponController>(weapon));
  }
}

void PlayerObject::set_local_player() noexcept {
  for (auto& behavior_ref : get_components<BehaviorScript>()) {
      auto& behavior = behavior_ref.get().behavior();

      if (auto movement = dynamic_cast<PlayerMovementBehavior*>(&behavior); movement != nullptr) {
        movement->set_local_player();
      }
  }
};

void PlayerObject::set_controllable() noexcept {
  for (auto& behavior_ref : get_components<BehaviorScript>()) {
      auto& behavior = behavior_ref.get().behavior();

      if (auto movement = dynamic_cast<PlayerMovementBehavior*>(&behavior); movement != nullptr) {
        movement->set_controllable();
      }
  }
}

bool PlayerObject::is_multiplayer(GameObject& obj) {
  auto network_identity = obj.get_component<NetworkIdentity>();
  if (network_identity.has_value()) {
    return network_identity->get().uuid().empty() == false;
  }

  return false;
}

bool PlayerObject::is_local(GameObject& obj) {
  auto network_identity = obj.get_component<NetworkIdentity>();
  if (network_identity.has_value() && !network_identity->get().uuid().empty()) {
    auto uuid = network_identity->get().uuid();
    auto multiplayer_uuid = Engine::instance().services->get_service<MultiplayerService>().get().get_uuid();

    return multiplayer_uuid == uuid;
  }

  return true;
}

void PlayerObject::user_name(std::string user_name) {
  user_name_ = user_name;
}

const std::string& PlayerObject::user_name() const {
  return user_name_;
}
