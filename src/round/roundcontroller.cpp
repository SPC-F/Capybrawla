#include <engine/public/scene.h>
#include <game/character/player_controller.h>

#include <game/round/roundcontroller.h>

RoundController::RoundController() : Behavior() {}

void RoundController::on_awake() {}
void RoundController::on_update(float dt) {}

void on_player_death(PlayerObject &player) {
  // Handle player death logic here
}

void RoundController::add_player(PlayerObject& player) {
  for (auto behavior : player.get_components<BehaviorScript>()) {
    if (auto pc = dynamic_cast<PlayerControllerBehavior *>(&behavior.get())) {
      pc->on_lives_changed([&player, &pc](int old_lives, int new_lives) {
        if (pc->is_alive()) {
          return;
        }
        on_player_death(player);
      });
    }
  }

  players.push_back(player);
}
void RoundController::remove_player(PlayerObject &player) {
  std::erase_if(players,
                [&player](const std::reference_wrapper<PlayerObject> &p) {
                  return p.get().id() == player.id();
                });
}

void RoundController::round_end() const {
  for (auto &callback : round_end_callbacks) {
    callback();
  }
}

void RoundController::on_round_end(const round_end_callback_t &callback) {
  round_end_callbacks.push_back(callback);
}