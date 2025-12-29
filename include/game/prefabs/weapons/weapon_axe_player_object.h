#pragma once

#include <engine/public/scene.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class WeaponAxePlayerObject final : public GameObject {
public:
  explicit WeaponAxePlayerObject(Scene& scene, GameObject& parent);
  ~WeaponAxePlayerObject() override = default;
};
