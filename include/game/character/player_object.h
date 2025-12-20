#pragma once
#include "engine/public/gameObject.h"
#include "engine/public/util/vector3.h"
#include "engine/public/util/point.h"
#include "engine/public/components/behaviorscript.h"
#include "game/character/player_movement_behavior.h"

class PlayerObject final : public GameObject {
public:
  explicit PlayerObject(Scene& scene, Vector3 initial_pos, bool is_local_player = true);
  ~PlayerObject() override = default;

  void set_controllable() {
    this->add_component<BehaviorScript>(std::make_unique<PlayerMovementBehavior>(
        default_height_, default_height_ / 2.0f, default_offset_,
        Point{default_x_offset_, default_height_ / 2.4 * scale_factor_}));
  };

private:
  const float scale_factor_ = 2.0f;
  const float default_height_ = 28 * scale_factor_;
  const float default_x_offset_ = 6 * scale_factor_;
  const Point default_offset_ = {default_x_offset_, 4 * scale_factor_};
};
