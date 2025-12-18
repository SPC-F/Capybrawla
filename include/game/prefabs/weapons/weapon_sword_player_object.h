#pragma once

#include <engine/public/scene.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class WeaponSwordPlayerObject final : public GameObject {
public:
  explicit WeaponSwordPlayerObject(Scene& scene, GameObject& parent);
  ~WeaponSwordPlayerObject() override = default;

};
