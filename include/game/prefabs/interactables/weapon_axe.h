#pragma once

#include <engine/public/gameObject.h>

class WeaponAxePrefab final : public GameObject {
public:
    explicit WeaponAxePrefab(Scene& scene, bool is_dropped = false, Vector3 position = Vector3{0,0,0});
    ~WeaponAxePrefab() override = default;
};
