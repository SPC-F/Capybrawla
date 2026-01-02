#pragma once

#include <engine/public/scene.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class WeaponBatPlayerObject final : public GameObject {
public:
  explicit WeaponBatPlayerObject(Scene& scene, GameObject& parent);
  ~WeaponBatPlayerObject() override = default;
};
