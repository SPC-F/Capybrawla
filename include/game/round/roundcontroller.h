#pragma once

#include <functional>
#include <random>
#include <map>
#include <memory>
#include <engine/public/behavior.h>
#include <engine/public/scene.h>

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
  std::map<std::string, std::reference_wrapper<GameObject>> player_info_components;
  std::vector<Vector3> spawn_positions_;

  void on_player_death(const PlayerObject &player);
  void round_end() const;
  void realign_player_info_positions();

public:
  explicit RoundController(std::vector<Vector3> spawn_positions);
  void on_awake() override;
  void on_update(float dt) override;

  void add_player(PlayerObject& player, Scene& scene);
  void remove_player(PlayerObject &player);

  bool is_player_registered(const PlayerObject& player) const;

  void on_round_end(const round_end_callback_t &callback);

  [[nodiscard]] std::vector<Vector3> spawn_positions() const;
  void spawn_positions(std::vector<Vector3> positions);
};
