#include "engine/core/engine.h"
#include "engine/input/i_input_provider.h"
#include "engine/input/input_manager.h"

#include <game/character/player_movement_behavior.h>

PlayerMovementBehavior::PlayerMovementBehavior()
    : Behavior(), rigidbody_(std::nullopt), animator_(std::nullopt),
      sprite_(std::nullopt), horizontal_speed_(5.0f), jumping_speed_(2.0f),
      dropping_speed_(1.0f), is_dropping_(false), is_jumping_(false) {}

void PlayerMovementBehavior::on_start() {
  rigidbody_ = this->get_component<Rigidbody2D>();
  animator_ = this->get_component<Animator>();
  animator_ = this->get_component<Animator>();
  sprite_ = this->get_component<Sprite>();
}

void PlayerMovementBehavior::on_update(float dt) {
  const IInputProvider &provider =
      Engine::instance().services->get_service<InputManager>().get().provider();
  GameObject &owner = this->game_object();

  auto rigidBodyOpt = owner.get_component<Rigidbody2D>();

  if (!rigidBodyOpt.has_value()) {
    return;
  }

  const bool pressing_or_holding_down =
      provider.is_key_pressed(KeyCode::s) || provider.is_key_held(KeyCode::s);

  const bool pressing_or_holding_jump =
      provider.is_key_pressed(KeyCode::space) ||
      provider.is_key_pressed(KeyCode::w);

  auto &rigidbody = rigidBodyOpt.value().get();
  float velocity_x = 0;
  float applied_force_y = 0.0f;

  if (!pressing_or_holding_down) {
    is_dropping_ = false;
  }
  if (!pressing_or_holding_jump) {
    is_jumping_ = false;
  }

  if (pressing_or_holding_jump && !is_jumping_) {
    applied_force_y -= 1 * jumping_speed_;
    is_jumping_ = true;
  }
  if (provider.is_key_pressed(KeyCode::a) || provider.is_key_held(KeyCode::a)) {
    velocity_x -= 1 * horizontal_speed_;
  }
  if (pressing_or_holding_down) {
    is_dropping_ = true;
    applied_force_y += 1 * dropping_speed_;
  }
  if (provider.is_key_pressed(KeyCode::d) || provider.is_key_held(KeyCode::d)) {
    velocity_x += 1 * horizontal_speed_;
  }

  Vector3 velocity = rigidbody.velocity();
  velocity.x = velocity_x;

  rigidbody.velocity(velocity);
  rigidbody.apply_force({0, applied_force_y, 0});
}