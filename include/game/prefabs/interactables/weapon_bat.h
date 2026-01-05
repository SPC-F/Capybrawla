#pragma once

#include <engine/public/gameObject.h>

class WeaponBatPrefab final : public GameObject {
public:
    explicit WeaponBatPrefab(Scene& scene, bool is_dropped = false, Vector3 position = Vector3{0,0,0});
    ~WeaponBatPrefab() override = default;
};
