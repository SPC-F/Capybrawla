
#include <engine/util/uuid.h>
#include <game/character/player_controller.h>

PlayerControllerBehavior::PlayerControllerBehavior(): PlayerControllerBehavior(100, 100) {}
PlayerControllerBehavior::PlayerControllerBehavior(const int max_health, const int start_health_): max_health_(max_health), health_ { start_health_ }, lives_(3) {}

void PlayerControllerBehavior::on_awake() {}
void PlayerControllerBehavior::on_start() {}
void PlayerControllerBehavior::on_update(float dt) {}

int PlayerControllerBehavior::health() const {
  return health_;
}
bool PlayerControllerBehavior::is_alive() const { return health_ > 0; }
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
int PlayerControllerBehavior::max_health() const { return max_health_; }
void PlayerControllerBehavior::max_health(const int max_health) {
  max_health_ = max_health;
}
lib::Subscription PlayerControllerBehavior::on_health_changed(
    const health_changed_callback_t &callback) {

  const auto id = uuid::generate_uuid_v4();
  health_changed_signals_.emplace_back(id, callback);
  return lib::Subscription([this, id]() {
      std::erase_if(health_changed_signals_,
          [&](const lib::Signal<int, int>& signal) {
              return signal.id() == id;
          });
  });
}

void PlayerControllerBehavior::notify_health_changed(const int old_health) const {
  for (const auto& signal : health_changed_signals_) {
    signal.envoke(old_health, health_);
  }
}

int PlayerControllerBehavior::lives() const { return lives_; }
void PlayerControllerBehavior::lives(const int lives) {
  const int old_lives = lives_;
  lives_ = lives;
  notify_lives_changed(old_lives);
}
lib::Subscription PlayerControllerBehavior::on_lives_changed(
    const lives_changed_callback_t &callback) {

  const auto id = uuid::generate_uuid_v4();
  lives_changed_signals_.emplace_back(id, callback);

  return lib::Subscription([this, id]() {
      std::erase_if(lives_changed_signals_,
          [&](const lib::Signal<int, int>& signal) {
              return signal.id() == id;
          });
  });
}
void PlayerControllerBehavior::notify_lives_changed(const int old_lives) const {
  for (const auto &signal : lives_changed_signals_) {
    signal.envoke(old_lives, lives_);
  }
}