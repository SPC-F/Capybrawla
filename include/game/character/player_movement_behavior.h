#pragma once
#include "engine/public/behavior.h"
#include "engine/public/components/animator.h"
#include "engine/public/components/colliders/box_collider_2d.h"
#include "engine/public/components/rigidbody_2d.h"
#include "engine/public/components/sprite.h"
#include "engine/public/scene.h"

#include <game/character/player_movement_types.h>

class PlayerMovementBehavior final : public Behavior {
private:
  std::optional<std::reference_wrapper<Rigidbody2D>> rigidbody_opt_;
  std::optional<std::reference_wrapper<Animator>> animator_opt_;
  std::optional<std::reference_wrapper<Sprite>> sprite_opt_;
  std::optional<std::reference_wrapper<BoxCollider2D>> box_collider_opt_;

  float latest_dt_;

  float horizontal_velocity_;
  float jumping_force_;
  float dropping_speed_;
  float double_jump_force_;
  float velocity_y_threshold_;
  float knockback_decay_;
  Vector3 knockback_velocity_;

  bool is_grounded_;
  bool is_crouching_;
  bool is_jumping_;
  bool is_double_jumping_;
  bool is_walking_;

  // Used when updating the player objects of peers during which we don't want to reset these states.
  bool crouch_;
  bool jump_;
  bool move_left_;
  bool move_right_;

  bool send_empty_message_; // First message sent after no movement has been detected to clean animation states on peers

  float default_standing_height_;
  float default_crouching_height_;

  Point default_standing_offset_;
  Point default_crouching_offset_;

  bool is_local_player_ = false;
  bool is_controllable_ = false;

  [[nodiscard]] bool player_has_required_components() const;

public:
  PlayerMovementBehavior();
  PlayerMovementBehavior(const float default_standing_height,
                         const float default_crouching_height,
                         Point default_standing_offset,
                         Point default_crouching_offset);
  PlayerMovementBehavior(float horizontal_velocity, float jumping_force,
                         float dropping_speed, float double_jump_force,
                         float velocity_y_threshold,
                         const float default_standing_height,
                         const float default_crouching_height,
                         Point default_standing_offset,
                         Point default_crouching_offset);

  ~PlayerMovementBehavior() override = default;

  [[nodiscard]] float horizontal_velocity() const;
  void horizontal_velocity(const float speed);

  [[nodiscard]] float jumping_force() const;
  void jumping_force(const float speed);

  [[nodiscard]] float dropping_speed() const;
  void dropping_speed(const float speed);

  [[nodiscard]] float double_jump_force() const;
  void double_jump_force(const float speed);

  [[nodiscard]] float velocity_y_threshold() const;
  void velocity_y_threshold(const float threshold);

  void apply_knockback(const Point& force);

  [[nodiscard]] bool is_crouching() const;
  [[nodiscard]] bool is_jumping() const;
  [[nodiscard]] bool is_double_jumping() const;
  [[nodiscard]] bool is_walking() const;

  void set_local_player() noexcept; // If this player object belongs to you
  void set_controllable() noexcept; // If it should listen to user input

  void handle_movement(const std::vector<PlayerMovementTypes>& movement);
  void gather_input(std::vector<PlayerMovementTypes>& movement);
  void set_movement_flags(const std::vector<PlayerMovementTypes>& movement);
  void apply_physics(const std::vector<PlayerMovementTypes>& movement);
  void update_animation();
  void send_movement_if_needed(const std::vector<PlayerMovementTypes>& movement);

  void on_start() override;
  void on_update(float dt) override;
};
