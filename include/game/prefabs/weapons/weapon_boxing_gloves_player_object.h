#pragma once

#include <engine/public/scene.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class WeaponBoxingGlovesPlayerObject final : public GameObject {
public:
  explicit WeaponBoxingGlovesPlayerObject(Scene& scene, GameObject& parent);
  ~WeaponBoxingGlovesPlayerObject() override = default;

  constexpr static int DAMAGE = 5;
};
