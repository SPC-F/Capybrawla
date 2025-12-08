#pragma once
#include "engine/public/behavior.h"
#include "engine/public/components/animator.h"
#include "engine/public/components/rigidbody_2d.h"
#include "engine/public/components/sprite.h"
#include "engine/public/scene.h"

class PlayerMovementBehavior final : public Behavior {
private:
  std::optional<std::reference_wrapper<Rigidbody2D>> rigidbody_;
  std::optional<std::reference_wrapper<Animator>> animator_;
  std::optional<std::reference_wrapper<Sprite>> sprite_;

  float horizontal_speed_;
  float jumping_speed_;
  float dropping_speed_;
  bool is_dropping_;
  bool is_jumping_;

public:

  [[nodiscard]] float horizontal_speed() const {
    return horizontal_speed_;
  }
  void horizontal_speed(const float speed) {
    horizontal_speed_ = speed;
  }

  [[nodiscard]] float jumping_speed() const {
    return jumping_speed_;
  }
  void jumping_speed(const float speed) {
    jumping_speed_ = speed;
  }

  [[nodiscard]] float dropping_speed() const {
    return dropping_speed_;
  }
  void dropping_speed(const float speed) {
    dropping_speed_ = speed;
  }

  [[nodiscard]] bool is_dropping() const {
    return is_dropping_;
  }
  [[nodiscard]] bool is_jumping() const {
    return is_jumping_;
  }

  explicit PlayerMovementBehavior();
  ~PlayerMovementBehavior() override = default;
  void on_start() override;
  void on_update(float dt) override;;
};
