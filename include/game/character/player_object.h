#pragma once
#include "engine/public/gameObject.h"
#include "engine/public/util/vector3.h"
#include "engine/public/util/point.h"
#include "engine/public/components/behaviorscript.h"
#include "game/character/player_movement_behavior.h"
#include "game/character/playerConstants.h"

class PlayerObject final : public GameObject {
public:
  explicit PlayerObject(Scene& scene, Vector3 initial_pos, bool is_local_player = true);
  ~PlayerObject() override = default;

  void set_local_player() noexcept;
  void set_controllable() noexcept;

  static bool is_multiplayer(GameObject& obj);
  static bool is_local(GameObject& obj);

  void user_name(std::string user_name);
  const std::string& user_name() const;

  void user_color(PlayerColor user_color);
  const PlayerColor user_color() const;

  const PlayerAnimationConstants& constants() const;

private:
  std::string user_name_;
  PlayerColor color_;
  PlayerAnimationConstants constants_;
};
