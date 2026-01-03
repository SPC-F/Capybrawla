#pragma once

#include <engine/public/gameObject.h>

class WeaponAxePrefab final : public GameObject {
public:
    explicit WeaponAxePrefab(Scene& scene);
    ~WeaponAxePrefab() override = default;
};
