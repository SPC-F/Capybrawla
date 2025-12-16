#pragma once
#include <engine/public/behavior.h>
#include <game/character/player_controller.h>

class PlayerOutOfBoundsBehavior final: public Behavior {
private:
  const float left_bound_;
  const float right_bound_;
  const float lower_bound_;
  const float upper_bound_;

public:
  explicit PlayerOutOfBoundsBehavior(float left_bound, float right_bound,
                                     float lower_bound, float upper_bound);
  ~PlayerOutOfBoundsBehavior() override = default;

  void on_update(float delta_time) override;
};