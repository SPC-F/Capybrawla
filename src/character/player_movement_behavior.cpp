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
  return rigidbody_opt_.has_value() && animator_opt_.has_value() &&
         sprite_opt_.has_value() && box_collider_opt_.has_value();
}

void PlayerMovementBehavior::on_update(float dt) {

  const IInputProvider &provider =
      Engine::instance().services->get_service<InputManager>().get().provider();

  if (!player_has_required_components())
    return;

  auto &rigidbody = rigidbody_opt_->get();
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
    sprite.texture("capybara_default");
  }

  if (crouch_input && !is_crouching_) {
    sprite.texture("capybara_duck");
    animator.pause();
    is_crouching_ = true;
  }

  if (!crouch_input && is_crouching_) {
    sprite.texture("capybara_default");
    // restore idle or walking depending on current state
    if (is_walking_)
      animator.play("WALKcapybara", true);
    else
      animator.pause();

    is_crouching_ = false;
  }

  if (walking && !is_walking_) {
    animator.play("WALKcapybara", true);
    is_walking_ = true;
  } else if (!walking && is_walking_) {
    animator.pause();
    is_walking_ = false;
  }

  float velocity_x = 0.0f;
  float applied_force_y = 0.0f;

  if (crouch_input) {
    applied_force_y += dropping_speed_;

    if (!is_crouching_) {
      sprite.texture("capybara_duck");
      animator.pause();
      is_crouching_ = true;
    }
  }

  if (walk_left_input) {
    velocity_x -= horizontal_speed_;
    if (!sprite.flip_x())
      sprite.flip_x(true);
  }

  if (walk_right_input) {
    velocity_x += horizontal_speed_;
    if (sprite.flip_x())
      sprite.flip_x(false);
  }

  if (jump_input && !is_jumping_) {
    applied_force_y -= jumping_speed_;
    is_jumping_ = true;

    sprite.texture("capybara_default");
    animator.pause();
  } else if (jump_input && !is_double_jumping_) {
    applied_force_y -= (jumping_speed_ / 1.5f);
    is_double_jumping_ = true;

    sprite.texture("capybara_default");
    animator.pause();
  }

  Vector3 velocity = rigidbody.velocity();
  velocity.x = velocity_x;

  rigidbody.velocity(velocity);
  rigidbody.apply_force({0, applied_force_y, 0});
}