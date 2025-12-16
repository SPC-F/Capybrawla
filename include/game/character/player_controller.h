#pragma once

#include "engine/public/components/behaviorscript.h"
#include <functional>
using health_changed_callback_t = std::function<void(const int old_health, const int new_health)>;

class PlayerControllerBehavior final : public Behavior {
private:
  int health_;
  std::vector<health_changed_callback_t> health_changed_callbacks_;
  void notify_health_changed(const int old_health) const;
public:
  explicit PlayerControllerBehavior();
  explicit PlayerControllerBehavior(int health);
  ~PlayerControllerBehavior() override = default;

  void on_awake() override;
  void on_start() override;
  void on_update(float dt) override;

  int health() const;
  void health(const int health);
  void damage(const int dmg);
  void heal(const int hp);
  bool is_alive() const;

  void on_health_changed(const health_changed_callback_t& callback);
};