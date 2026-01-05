#pragma once

#include <engine/public/components/behaviorscript.h>
#include <functional>
#include <lib/subscription.h>

using health_changed_callback_t = std::function<void(const int old_health, const int new_health)>;
using lives_changed_callback_t = std::function<void(const int old_lives, const int new_lives)>;

class PlayerController final : public Behavior {
private:
  int max_health_;
  int health_;
  std::vector<lib::Signal<int, int>> health_changed_signals_;
  void notify_health_changed(int old_health) const;

  int max_lives_{3};
  int lives_;
  std::vector<lib::Signal<int, int>> lives_changed_signals_;
  void notify_lives_changed(int old_lives) const;

public:
  explicit PlayerController();
  explicit PlayerController(int max_health, int start_health, int max_lives);
  ~PlayerController() override = default;

  void on_update(float dt) override;

  [[nodiscard]] int health() const;
  void health(int health);
  void damage(int dmg);
  void heal(int hp);
  [[nodiscard]] bool is_alive() const;
  lib::Subscription on_health_changed(const health_changed_callback_t &callback);

  [[nodiscard]] int max_health() const;
  void max_health(int max_health);

  [[nodiscard]] int lives() const;
  void lives(int lives);
  lib::Subscription on_lives_changed(const lives_changed_callback_t &callback);
  [[nodiscard]] int max_lives() const;

  [[nodiscard]] bool is_soft_dead() const;
  [[nodiscard]] bool is_hard_dead() const;

  void hit(int damage);

  void on_destroy() override;

  void mark_network_dirty() noexcept;

  void on_serialize(std::vector<uint8_t>& out) const override;
  void on_deserialize(const std::vector<uint8_t>& data, size_t& offset) override;
};