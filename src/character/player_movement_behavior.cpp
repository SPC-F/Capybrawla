#include "engine/core/engine.h"
#include "engine/core/rendering/assetService.h"
#include "engine/input/i_input_provider.h"
#include "engine/input/input_manager.h"

#include <game/character/player_movement_behavior.h>

PlayerMovementBehavior::PlayerMovementBehavior()
    : Behavior(), rigidbody_opt_(std::nullopt), animator_opt_(std::nullopt),
      sprite_opt_(std::nullopt), box_collider_opt_(std::nullopt),
      horizontal_speed_(12.0f), jumping_speed_(6.0f), dropping_speed_(0.15f),
      is_crouching_(false), is_jumping_(false), is_double_jumping_(false),
      is_walking_(false) {}

void PlayerMovementBehavior::on_start() {
  rigidbody_opt_ = this->get_component<Rigidbody2D>();
  animator_opt_ = this->get_component<Animator>();
  sprite_opt_ = this->get_component<Sprite>();
  box_collider_opt_ = this->get_component<BoxCollider2D>();

  if (!player_has_required_components()) {
    throw std::runtime_error(
        "player_has_required_components() returned false in "
        "PlayerMovementBehavior::on_start");
  }

  box_collider_opt_.value().get().add_on_collision_enter(
      [&](Collider2D &self, Collider2D &other) {
        const auto parent_opt = self.parent();
        const auto other_parent_opt = other.parent();
        if (!parent_opt.has_value() || !other_parent_opt.has_value())
          return;

        const Transform &self_transform = parent_opt->get().transform();
        const Transform &other_transform = other_parent_opt->get().transform();

        if (self_transform.position().x > other_transform.position().x) {
          return;
        }

        is_crouching_ = false;
        is_jumping_ = false;
        is_double_jumping_ = false;

        Vector3 velocity = rigidbody_opt_->get().velocity();
        rigidbody_opt_->get().velocity({velocity.x, 0.0f, velocity.z});
      });
}

bool PlayerMovementBehavior::player_has_required_components() const {
  return rigidbody_opt_.has_value() || animator_opt_.has_value() ||
         sprite_opt_.has_value() || box_collider_opt_.has_value();
}

void PlayerMovementBehavior::on_update(float dt) {
  const IInputProvider &provider =
      Engine::instance().services->get_service<InputManager>().get().provider();
  GameObject &owner = this->game_object();

  if (!player_has_required_components()) {
    return;
  }

  // Register the inputs as readable booleans => separate input logic from action
  const bool crouch_input =
      provider.is_key_pressed(KeyCode::s) ||
      provider.is_key_held(KeyCode::s);

  const bool jump_input =
      provider.is_key_pressed(KeyCode::space) ||
      provider.is_key_pressed(KeyCode::w);

  const bool walk_left_input =
      provider.is_key_pressed(KeyCode::a) ||
      provider.is_key_held(KeyCode::a);

  const bool walk_right_input =
      provider.is_key_pressed(KeyCode::d) ||
      provider.is_key_held(KeyCode::d);

  // Checks for animation resetting beforehand...

  const bool is_moving =
      crouch_input || jump_input || walk_left_input || walk_right_input;

  // if the player is not moving...
  // We base movement on input because we could lack behind a frame if we didn't...
  if (!is_moving) {
    sprite_opt_->get().texture("capybara_default");
  }

  if (!crouch_input) {
    is_crouching_ = false;
  }
  if (!walk_left_input && !walk_right_input) {
    is_walking_ = false;
    animator_opt_->get().pause();
  }

  // Checking for movement inputs...

  auto &rigidbody = rigidbody_opt_.value().get();
  float velocity_x = 0;
  float applied_force_y = 0.0f;

  if (crouch_input) {
    applied_force_y += 1 * dropping_speed_;
    is_crouching_ = true;
    sprite_opt_->get().texture("capybara_duck");
    animator_opt_->get().pause();
  }
  if (walk_left_input) {
    velocity_x -= 1 * horizontal_speed_;
    // Flip the sprite
    animator_opt_->get().play("WALKcapybara");
  }
  if (walk_right_input) {
    velocity_x += 1 * horizontal_speed_;
    animator_opt_->get().play("WALKcapybara");
  }
  if (jump_input && !is_jumping_) {
    applied_force_y -= 1 * jumping_speed_;
    is_jumping_ = true;
    sprite_opt_->get().texture("capybara_default");
    animator_opt_->get().pause();
  } else if (jump_input && !is_double_jumping_) {
    applied_force_y -= 1 * (jumping_speed_ / 1.5);
    is_double_jumping_ = true;
    sprite_opt_->get().texture("capybara_default");
    animator_opt_->get().pause();
  }

  // Apply the calculated velocities and forces
  Vector3 velocity = rigidbody.velocity();
  velocity.x = velocity_x;

  rigidbody.velocity(velocity);
  rigidbody.apply_force({0, applied_force_y, 0});
}