#include <game/character/player_controller.h>
#include <game/character/playerConstants.h>
#include <game/prefabs/character/dying_capybara_object.h>


#include <engine/network/snapshot.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/animator.h>
#include <engine/public/scene.h>
#include <engine/util/uuid.h>

#include <engine/audio/audio_service.h>
#include <engine/core/engine.h>

PlayerController::PlayerController(): PlayerController(100, 100) {}
PlayerController::PlayerController(const int max_health, const int start_health): max_health_(max_health), health_ { start_health }, lives_(3) {}

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

  mark_network_dirty();

  notify_health_changed(old_health);

  auto& audio_controller = Engine::instance().services->get_service<AudioService>().get();
  audio_controller.play_sound("player_hit", 0.15f, false);
}
void PlayerController::heal(const int hp) {
  const int old_health = health_;
  if (health_ + hp > max_health_) {
    health_ = max_health_;
  } else {
    health_ += hp;
  }

  mark_network_dirty();

  notify_health_changed(old_health);
}
int PlayerController::max_health() const { return max_health_; }
void PlayerController::max_health(const int max_health) {
  max_health_ = max_health;

  mark_network_dirty();
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

  mark_network_dirty();

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

void PlayerController::hit(int damage) {
  auto animator = get_component<Animator>();
  
  if (animator) {
    animator->get().is_non_interruptible(true);
    animator->get().play(PlayerConstants::HIT_ANIMATION, false);
  }

  this->damage(damage);
}

void PlayerController::on_destroy() {
  health_changed_signals_.clear();
  lives_changed_signals_.clear();
}

void PlayerController::mark_network_dirty() noexcept {
  auto& parent = game_object();
    if (auto network_id = parent.get_component<NetworkIdentity>()) {
      network_id.value().get().mark_dirty();
    }
}

void PlayerController::on_serialize(std::vector<uint8_t>& out) const {
  snapshot::write_bytes(out, &max_health_, sizeof(int));
  snapshot::write_bytes(out, &health_, sizeof(int));
  snapshot::write_bytes(out, &lives_, sizeof(int));
};

void PlayerController::on_deserialize(const std::vector<uint8_t>& data, size_t& offset) {
  int old_health = health_;
  int old_lives = lives_;
  if (!snapshot::read_bytes(data, offset, &max_health_, sizeof(float))) return;
  if (!snapshot::read_bytes(data, offset, &health_, sizeof(float))) return;
  if (!snapshot::read_bytes(data, offset, &lives_, sizeof(float))) return;

  if (old_health != health_)
    notify_health_changed(old_health);

  if (old_lives != lives_)
    notify_lives_changed(old_lives);
};