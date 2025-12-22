#pragma once
#include "engine/public/behavior.h"
#include "engine/public/components/animator.h"
#include "engine/public/components/colliders/box_collider_2d.h"
#include "engine/public/components/rigidbody_2d.h"
#include "engine/public/components/sprite.h"
#include "engine/public/scene.h"

class PlayerMovementBehavior final : public Behavior {
private:
  std::optional<std::reference_wrapper<Rigidbody2D>> rigidbody_opt_;
  std::optional<std::reference_wrapper<Animator>> animator_opt_;
  std::optional<std::reference_wrapper<Sprite>> sprite_opt_;
  std::optional<std::reference_wrapper<BoxCollider2D>> box_collider_opt_;

  float horizontal_velocity_;
  float jumping_force_;
  float dropping_speed_;
  float double_jump_force_;
  float velocity_y_threshold_;

  bool is_crouching_;
  bool is_jumping_;
  bool is_double_jumping_;
  bool is_walking_;

  float default_standing_height_;
  float default_crouching_height_;

  Point default_standing_offset_;
  Point default_crouching_offset_;

  bool is_local_player_;

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

  [[nodiscard]] bool is_crouching() const;
  [[nodiscard]] bool is_jumping() const;
  [[nodiscard]] bool is_double_jumping() const;
  [[nodiscard]] bool is_walking() const;

  void set_local_player() noexcept;

  void on_start() override;
  void on_update(float dt) override;
};
