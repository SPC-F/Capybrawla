#include <game/character/player_outofbounds_behavior.h>

PlayerOutOfBoundsBehavior::PlayerOutOfBoundsBehavior(const float left_bound,
                                                     const float right_bound,
                                                     const float lower_bound,
                                                     const float upper_bound)
    : left_bound_{-50.0f}, right_bound_{50.0f}, lower_bound_{-10.0f},
      upper_bound_{50.0f} {}

void PlayerOutOfBoundsBehavior::on_update(float delta_time) {
  if (const Vector3 position = game_object().transform().position();
      !(position.x < left_bound_ || position.x > right_bound_ ||
        position.y < lower_bound_ || position.y > upper_bound_)) {
    return;
  }

  for (auto &behavior : game_object().get_components<BehaviorScript>()) {
    if (const auto player_controller =
            dynamic_cast<PlayerControllerBehavior *>(&behavior.get())) {
      player_controller->damage(player_controller->health());
      break;
    }
  }
}