
#include <game/character/player_controller.h>

PlayerControllerBehavior::PlayerControllerBehavior(): PlayerControllerBehavior(100) {}
PlayerControllerBehavior::PlayerControllerBehavior(const int health): health_ { health } {}

void PlayerControllerBehavior::on_awake() {}
void PlayerControllerBehavior::on_start() {}
void PlayerControllerBehavior::on_update(float dt) {}

int PlayerControllerBehavior::health() const {
  return health_;
}
void PlayerControllerBehavior::health(const int health) {
  const int old_health = health_;
  health_ = health;
  notify_health_changed(old_health);
}
void PlayerControllerBehavior::damage(const int dmg) {
  const int old_health = health_;
  health_ -= dmg;
  notify_health_changed(old_health);
}
void PlayerControllerBehavior::heal(const int hp) {
  const int old_health = health_;
  health_ += hp;
  notify_health_changed(old_health);
}

bool PlayerControllerBehavior::is_alive() const { return health_ > 0; }

void PlayerControllerBehavior::on_health_changed(
    const health_changed_callback_t &callback) {
  health_changed_callbacks_.push_back(callback);
}
void PlayerControllerBehavior::notify_health_changed(const int old_health) const {
  for (const auto& callback : health_changed_callbacks_) {
    callback(old_health, health_);
  }
}