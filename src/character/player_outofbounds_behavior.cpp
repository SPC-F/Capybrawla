#include <game/character/player_outofbounds_behavior.h>
#include <game/character/player_controller.h>

PlayerOutOfBoundsBehavior::PlayerOutOfBoundsBehavior(const float left_bound,
                                                     const float right_bound,
                                                     const float lower_bound,
                                                     const float upper_bound)
    : left_bound_{left_bound}, right_bound_{right_bound}, lower_bound_{lower_bound},
      upper_bound_{upper_bound} {}

void PlayerOutOfBoundsBehavior::on_update(float delta_time) {
  const Vector3 position = game_object().transform().position();
  if (!(position.x < left_bound_ || position.x > right_bound_ ||
        position.y < lower_bound_ || position.y > upper_bound_)) {
    return;
  }
  const auto& components = game_object().get_components<BehaviorScript>();
  for (auto behavior : components) {
    if (PlayerController* player_controller = dynamic_cast<PlayerController*>(&behavior.get().behavior())) {
      if (player_controller->is_alive()) {
        player_controller->damage(player_controller->health());
      }

      break;
    }
  }
}