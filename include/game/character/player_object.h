#pragma once
#include "engine/public/gameObject.h"
#include "engine/public/util/vector3.h"

class PlayerObject final : public GameObject {
public:
  explicit PlayerObject(Scene& scene, Vector3 initial_pos, bool is_local_player = true);
  ~PlayerObject() override = default;
};
