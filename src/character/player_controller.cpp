
#include <engine/util/uuid.h>
#include <game/character/player_controller.h>

#include <engine/audio/audio_service.h>
#include <engine/core/engine.h>

PlayerController::PlayerController(): PlayerController(100, 100) {}
PlayerController::PlayerController(const int max_health, const int start_health): max_health_(max_health), health_ { start_health }, lives_(3) {}

void PlayerController::on_awake() {}
void PlayerController::on_start() {}
void PlayerController::on_update(float dt) {}

int PlayerController::health() const {
  return health_;
}
bool PlayerController::is_alive() const { return health_ > 0; }
void PlayerController::health(const int health) {
  const int old_health = health_;
  health_ = health;
  notify_health_changed(old_health);
}
void PlayerController::damage(const int dmg) {
  const int old_health = health_;
  health_ -= dmg;
  notify_health_changed(old_health);

  auto& audio_controller = Engine::instance().services->get_service<AudioService>().get();
  audio_controller.play_sound("player_hit", 0.15f, false);
}
void PlayerController::heal(const int hp) {
  const int old_health = health_;
  health_ += hp;
  notify_health_changed(old_health);
}
int PlayerController::max_health() const { return max_health_; }
void PlayerController::max_health(const int max_health) {
  max_health_ = max_health;
}
lib::Subscription PlayerController::on_health_changed(
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

void PlayerController::notify_health_changed(const int old_health) const {
  for (const auto& signal : health_changed_signals_) {
    signal.invoke(old_health, health_);
  }
}

int PlayerController::lives() const { return lives_; }
void PlayerController::lives(const int lives) {
  const int old_lives = lives_;
  lives_ = lives;
  notify_lives_changed(old_lives);
}
lib::Subscription PlayerController::on_lives_changed(
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
void PlayerController::notify_lives_changed(const int old_lives) const {
  for (const auto &signal : lives_changed_signals_) {
    signal.invoke(old_lives, lives_);
  }
}

bool PlayerController::is_hard_dead() const {
  return lives_ <= 1 && health_ <= 0;
}

bool PlayerController::is_soft_dead() const {
  return lives_ >= 1 && health_ <= 0;
}

void PlayerController::on_destroy() {
  health_changed_signals_.clear();
  lives_changed_signals_.clear();
}