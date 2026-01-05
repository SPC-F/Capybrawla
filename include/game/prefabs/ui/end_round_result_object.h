#pragma once

#include <game/character/player_object.h>

#include <engine/public/gameObject.h>
#include <engine/public/scene.h>
#include <engine/public/util/vector3.h>

class EndRoundResultObject final : public GameObject {
public:
  explicit EndRoundResultObject(Scene& scene, std::optional<std::reference_wrapper<PlayerObject>> player_opt, bool is_draw = false);
  ~EndRoundResultObject() override = default;

};
