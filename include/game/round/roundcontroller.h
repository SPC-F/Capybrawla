#pragma once

#include <functional>
#include <random>
#include <map>
#include <memory>
#include <engine/public/behavior.h>

#include <game/character/player_object.h>
#include <lib/subscription.h>

using round_end_callback_t = std::function<void()>;

class RoundController final : public Behavior {
private:
  std::mt19937 gen_{std::random_device{}()};
  size_t last_spawn_index_ = 0;

  std::vector<round_end_callback_t> round_end_callbacks;
  std::vector<std::reference_wrapper<PlayerObject>> players;
  std::map<std::string, lib::Subscription> on_player_health_changed_subscriptions;
  std::vector<Vector3> spawn_positions_;

  void on_player_death(const PlayerObject &player);
  void round_end() const;

public:
  explicit RoundController(std::vector<Vector3> spawn_positions);
  void on_awake() override;
  void on_update(float dt) override;

  void add_player(PlayerObject& player);
  void remove_player(PlayerObject &player);

  void on_round_end(const round_end_callback_t &callback);

  [[nodiscard]] std::vector<Vector3> spawn_positions() const;
  void spawn_positions(std::vector<Vector3> positions);
};
