#pragma once

#include <engine/public/gameObject.h>

class WeaponSwordPrefab final : public GameObject {
public:
    explicit WeaponSwordPrefab(Scene& scene, bool is_dropped = false, Vector3 position = Vector3{0,0,0});
    ~WeaponSwordPrefab() override = default;
};
