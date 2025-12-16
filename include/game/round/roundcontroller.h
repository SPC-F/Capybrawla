#pragma once

#include <functional>
#include <memory>
#include <engine/public/behavior.h>

#include <game/character/player_object.h>

using round_end_callback_t = std::function<void()>;

class RoundController final : public Behavior {
private:
  std::vector<round_end_callback_t> round_end_callbacks;
  std::vector<std::reference_wrapper<PlayerObject>> players;

  void round_end() const;

public:
  explicit RoundController();
  void on_awake() override;
  void on_update(float dt) override;

  void add_player(PlayerObject& player);
  void remove_player(PlayerObject &player);

  void on_round_end(const round_end_callback_t &callback);
};
