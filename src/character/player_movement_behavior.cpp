#include "engine/core/engine.h"
#include "engine/core/rendering/assetService.h"
#include "engine/input/i_input_provider.h"
#include "engine/input/input_manager.h"
#include <engine/network/multiplayer_service.h>

#include <game/network/message_types.h>
#include <game/character/playerConstants.h>
#include <game/character/player_movement_behavior.h>

PlayerMovementBehavior::PlayerMovementBehavior()
    : PlayerMovementBehavior(32.0f, 18.0f, {}, {}) {}

PlayerMovementBehavior::PlayerMovementBehavior(
    const float default_standing_height, const float default_crouching_height,
    const Point default_standing_offset, const Point default_crouching_offset)
    : PlayerMovementBehavior(30.0f, 8.0f, 1.5f, 6.0f, 18.0f,
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
      velocity_y_threshold_(velocity_y_threshold), knockback_decay_(7.0f),

      // State flags
      is_grounded_(false), is_crouching_(false), is_jumping_(false),
      is_double_jumping_(false), is_walking_(false),

      // Default collider heights
      default_standing_height_(default_standing_height),
      default_crouching_height_(default_crouching_height),

      // Default collider offsets
      default_standing_offset_(default_standing_offset),
      default_crouching_offset_(default_crouching_offset) {}

void PlayerMovementBehavior::on_start() {
  rigidbody_opt_    = get_component<Rigidbody2D>();
  animator_opt_     = get_component<Animator>();
  sprite_opt_       = get_component<Sprite>();
  box_collider_opt_ = get_component<BoxCollider2D>();

  if (!player_has_required_components()) {
    throw std::runtime_error("PlayerMovementBehavior missing components");
  }

  auto& collider = box_collider_opt_->get();

  collider.add_on_collision_enter(
    [&](Collider2D& self, Collider2D& other) {
      if (other.parent()->get().tag() != "Ground")
        return;

      is_grounded_ = true;
      is_jumping_ = false;
      is_double_jumping_ = false;

      self.friction(0.6f);
    });

  collider.add_on_collision_exit(
    [&](Collider2D& self, Collider2D& other) {
      if (other.parent()->get().tag() == "Ground")
        is_grounded_ = false;
    });
}

bool PlayerMovementBehavior::player_has_required_components() const {
  return rigidbody_opt_.has_value() && animator_opt_.has_value() &&
         sprite_opt_.has_value() && box_collider_opt_.has_value();
}

void PlayerMovementBehavior::set_local_player() noexcept {
  is_local_player_ = true;
}

void PlayerMovementBehavior::set_controllable() noexcept {
  is_controllable_ = true;
}

void PlayerMovementBehavior::on_update(float dt) {
  latest_dt_ = dt;

  if (!is_controllable_) {
    apply_physics({});
    update_animation();
    return;
  }

  std::vector<PlayerMovementTypes> movement;
  gather_input(movement);

  handle_movement(movement);

  if (is_local_player_)
    send_movement_if_needed(movement);
}


void PlayerMovementBehavior::handle_movement(
  const std::vector<PlayerMovementTypes>& movement
) {
  set_movement_flags(movement);
  apply_physics(movement);
  update_animation();
}


void PlayerMovementBehavior::gather_input(
  std::vector<PlayerMovementTypes>& movement
) {
  const auto& input =
    Engine::instance().services
      ->get_service<InputManager>().get().provider();

  if (input.is_key_held(KeyCode::a))
    movement.push_back(PlayerMovementTypes::MOVE_LEFT);

  if (input.is_key_held(KeyCode::d))
    movement.push_back(PlayerMovementTypes::MOVE_RIGHT);

  if (input.is_key_held(KeyCode::s))
    movement.push_back(PlayerMovementTypes::CROUCH);

  if (input.is_key_pressed(KeyCode::space) ||
      input.is_key_pressed(KeyCode::w))
    movement.push_back(PlayerMovementTypes::JUMP);
}


void PlayerMovementBehavior::set_movement_flags(
  const std::vector<PlayerMovementTypes>& movement
) {
  auto has = [&](const std::vector<PlayerMovementTypes>& vec,
                 PlayerMovementTypes type) {
    return std::find(vec.begin(), vec.end(), type) != vec.end();
  };

  move_left_  = has(movement, PlayerMovementTypes::MOVE_LEFT);
  move_right_ = has(movement, PlayerMovementTypes::MOVE_RIGHT);
  crouch_     = has(movement, PlayerMovementTypes::CROUCH);
  jump_       = has(movement, PlayerMovementTypes::JUMP);
}

void PlayerMovementBehavior::apply_physics(
  const std::vector<PlayerMovementTypes>& movement
) {
  if (!player_has_required_components())
    return;

  auto& rb  = rigidbody_opt_->get();
  auto& col = box_collider_opt_->get();

  Vector3 velocity = rb.velocity();
  Vector3 force = {};

  float horizontal = 0.0f;
  if (!is_crouching_) {
    if (move_left_)  horizontal -= horizontal_velocity_;
    if (move_right_) horizontal += horizontal_velocity_;
  }

  velocity.x = horizontal + knockback_velocity_.x;
  velocity.y += knockback_velocity_.y;

  is_walking_ = (horizontal != 0.0f);

  if (crouch_ && is_grounded_) {
    if (!is_crouching_) {
      col.height(default_crouching_height_);
      col.offset(default_crouching_offset_);
    }
    is_crouching_ = true;
  } else {
    if (is_crouching_) {
      col.height(default_standing_height_);
      col.offset(default_standing_offset_);
    }
    is_crouching_ = false;
  }

  if (jump_) {
    if (is_grounded_) {
      force.y = -jumping_force_;
      is_grounded_ = false;
      is_double_jumping_ = false;
    }
    else if (!is_double_jumping_) {
      force.y = -double_jump_force_;
      is_double_jumping_ = true;
    }
  }

  // Optional fast-fall
  if (crouch_ && !is_grounded_) {
    force.y += dropping_speed_;
    is_crouching_ = true;
  }

  rb.velocity(velocity);
  rb.apply_force(force);

  knockback_velocity_ -= knockback_velocity_ * knockback_decay_ * latest_dt_;

  if (knockback_velocity_.length() < 0.01f) {
    knockback_velocity_ = {0, 0, 0};
  }
}


void PlayerMovementBehavior::update_animation() {
  auto& animator = animator_opt_->get();
  auto& sprite   = sprite_opt_->get();

  if (crouch_) {
    sprite.texture(PlayerConstants::CROUCHING_TEXTURE);
    return;
  }

  if ((jump_ || is_double_jumping_) && !is_grounded_) {
    sprite.texture(PlayerConstants::JUMPING_TEXTURE);
    return;
  }

  if (is_walking_ && is_grounded_ && !animator.is_playing()) {
    animator.play(PlayerConstants::WALKING_ANIMATION, true);
  }

  if (!crouch_ && !is_walking_ && is_grounded_) {
    animator.pause();
    sprite.texture(PlayerConstants::IDLE_TEXTURE);
  }

  if (move_left_)  sprite.flip_x(true);
  if (move_right_) sprite.flip_x(false);
}


void PlayerMovementBehavior::send_movement_if_needed(const std::vector<PlayerMovementTypes>& movement) {
  // If we're not detecting any input, send one final message stating that we are no longer moving.
  if (movement.empty()) {
    if (!send_empty_message_) return;
    send_empty_message_ = false;
  } else {
    send_empty_message_ = true;
  }

  // Send message containing all movement
  MultiplayerService& multiplayer_service =
    Engine::instance().services->get_service<MultiplayerService>().get();

  MsgUserMove body{};
  std::strncpy(body.uuid, multiplayer_service.get_uuid().c_str(), sizeof(body.uuid) - 1);
  body.size = movement.size();

  for (int i = 0; i < movement.size(); ++i) {
    body.movement[i] = static_cast<uint8_t>(movement[i]);
  }

  Message msg = serialize_message(body, CustomMessageTypes::USER_MOVE);
  multiplayer_service.send(msg);
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

void PlayerMovementBehavior::apply_knockback(const Point& force) {
  knockback_velocity_.x += force.x;
  knockback_velocity_.y += force.y;
}