#include "engine/core/engine.h"
#include "engine/core/rendering/assetService.h"
#include "engine/input/i_input_provider.h"
#include "engine/input/input_manager.h"

#include <game/character/playerConstants.h>
#include <game/character/player_movement_behavior.h>

PlayerMovementBehavior::PlayerMovementBehavior()
    : PlayerMovementBehavior(32.0f, 18.0f, {}, {}) {}

PlayerMovementBehavior::PlayerMovementBehavior(
    const float default_standing_height, const float default_crouching_height,
    const Point default_standing_offset, const Point default_crouching_offset)
    : PlayerMovementBehavior(20.0f, 8.0f, 1.5f, 6.0f, 18.0f,
                             default_standing_height, default_crouching_height,
                             default_standing_offset,
                             default_crouching_offset) {}

PlayerMovementBehavior::PlayerMovementBehavior(
    const float horizontal_velocity, const float jumping_force,
    const float dropping_speed, const float double_jump_force,
    const float velocity_y_threshold, const float default_standing_height,
    const float default_crouching_height, const Point default_standing_offset, const Point default_crouching_offset)
    : Behavior(),
      // Required components
      rigidbody_opt_(std::nullopt), animator_opt_(std::nullopt),
      sprite_opt_(std::nullopt), box_collider_opt_(std::nullopt),

      // Configurable params for movement physics
      horizontal_velocity_(horizontal_velocity), jumping_force_(jumping_force),
      dropping_speed_(dropping_speed), double_jump_force_(double_jump_force),
      velocity_y_threshold_(velocity_y_threshold),

      // State flags
      is_crouching_(false), is_jumping_(false), is_double_jumping_(false),
      is_walking_(false),

      // Default collider heights
      default_standing_height_(default_standing_height),
      default_crouching_height_(default_crouching_height),

      // Default collider offsets
      default_standing_offset_(default_standing_offset),
      default_crouching_offset_(default_crouching_offset) {}

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

        // Are we grounded?
        if (self_transform.position().y > other_transform.position().y) {
          return;
        }

        // If we are both walking and jumping at the same time, resume walking
        if (is_walking_ && is_jumping_) {
          animator_opt_->get().play("capybara_default_walk_anim", true);
        }

        is_jumping_ = false;
        is_double_jumping_ = false;

        Vector3 velocity = rigidbody_opt_->get().velocity();
        rigidbody_opt_->get().velocity({velocity.x, 0.0f, velocity.z});
      });
}

bool PlayerMovementBehavior::player_has_required_components() const {
  return rigidbody_opt_.has_value() && animator_opt_.has_value() &&
         sprite_opt_.has_value() && box_collider_opt_.has_value();
}

void PlayerMovementBehavior::on_update(float dt) {
  const IInputProvider &provider =
      Engine::instance().services->get_service<InputManager>().get().provider();

  if (!player_has_required_components())
    return;

  auto &rigidbody = rigidbody_opt_->get();
  auto &box_collider = this->box_collider_opt_->get();
  auto &animator = animator_opt_->get();
  auto &sprite = sprite_opt_->get();

  const bool crouch_input = provider.is_key_held(KeyCode::s);
  const bool jump_input = provider.is_key_pressed(KeyCode::space) ||
                          provider.is_key_pressed(KeyCode::w);
  const bool walk_left_input = provider.is_key_held(KeyCode::a);
  const bool walk_right_input = provider.is_key_held(KeyCode::d);

  const bool walking = walk_left_input || walk_right_input;
  const bool is_moving = crouch_input || jump_input || walking;

  if (!is_moving) {
    sprite.texture("capybara_default_idle");
  }

  if (crouch_input && !is_crouching_) {
    box_collider.offset(default_crouching_offset_);
    box_collider.height(default_crouching_height_);

    sprite.texture("capybara_default_duck");
    animator.pause();

    is_crouching_ = true;
    is_walking_ = false;

  } else if (!crouch_input && is_crouching_) {
    box_collider.offset(default_standing_offset_);
    box_collider.height(default_standing_height_);

    sprite.texture("capybara_default_idle");
    if (is_walking_)
      animator.play("capybara_default_walk_anim", true);
    else
      animator.pause();

    is_crouching_ = false;
  }

  if (walking && !is_walking_ && !is_crouching_) {
    if (!is_jumping()) {
      animator.play(PlayerConstants::WALKING_ANIMATION, true);
    }

    is_walking_ = true;
  } else if (!walking && is_walking_) {
    animator.pause();
    is_walking_ = false;
  }

  float velocity_x = 0.0f;
  float applied_force_y = 0.0f;

  if (crouch_input) {
    applied_force_y += dropping_speed_;
  }

  if (walk_left_input && !is_crouching_) {
    velocity_x -= horizontal_velocity_;
    if (!sprite.flip_x())
      sprite.flip_x(true);
  }

  if (walk_right_input && !is_crouching_) {
    velocity_x += horizontal_velocity_;
    if (sprite.flip_x())
      sprite.flip_x(false);
  }

  const float current_velocity_y = rigidbody.velocity().y;
  if (jump_input && !is_jumping_) {
    if (std::abs(current_velocity_y) < velocity_y_threshold_) {
      rigidbody.velocity(
          {rigidbody.velocity().x, 0.0f, rigidbody.velocity().z});
    }

    applied_force_y -= jumping_force_;
    is_jumping_ = true;

    sprite.texture("capybara_default_idle");
    animator.pause();
  } else if (jump_input && !is_double_jumping_) {
    const float abs_velocity_y = std::abs(current_velocity_y);
    if (abs_velocity_y < velocity_y_threshold_) {
      rigidbody.velocity(
          {rigidbody.velocity().x, 0.0f, rigidbody.velocity().z});
    }

    applied_force_y -= double_jump_force_;
    // To make double jump feel smoother, we apply less force if the player is
    // already moving upwards
    applied_force_y +=
        std::clamp(current_velocity_y, 0.0f, velocity_y_threshold_);

    is_double_jumping_ = true;

    sprite.texture("capybara_default_idle");
    animator.pause();
  }

  Vector3 velocity = rigidbody.velocity();
  velocity.x = velocity_x;

  rigidbody.velocity(velocity);
  rigidbody.apply_force({0, applied_force_y, 0});
}

float PlayerMovementBehavior::horizontal_velocity() const {
  return horizontal_velocity_;
}
void PlayerMovementBehavior::horizontal_velocity(const float speed) {
  horizontal_velocity_ = speed;
}
float PlayerMovementBehavior::jumping_force() const {
  return jumping_force_; }
void PlayerMovementBehavior::jumping_force(const float speed) {
  jumping_force_ = speed;
}
float PlayerMovementBehavior::dropping_speed() const {
  return dropping_speed_; }
void PlayerMovementBehavior::dropping_speed(const float speed) {
  dropping_speed_ = speed;
}
float PlayerMovementBehavior::double_jump_force() const {
  return double_jump_force_;
}
void PlayerMovementBehavior::double_jump_force(const float speed) {
  double_jump_force_ = speed;
}
float PlayerMovementBehavior::velocity_y_threshold() const {
  return velocity_y_threshold_;
}
void PlayerMovementBehavior::velocity_y_threshold(const float threshold) {
  velocity_y_threshold_ = threshold;
}
bool PlayerMovementBehavior::is_crouching() const {
  return is_crouching_; }
bool PlayerMovementBehavior::is_jumping() const {
  return is_jumping_; }
bool PlayerMovementBehavior::is_double_jumping() const {
  return is_double_jumping_;
}
bool PlayerMovementBehavior::is_walking() const {
  return is_walking_; }