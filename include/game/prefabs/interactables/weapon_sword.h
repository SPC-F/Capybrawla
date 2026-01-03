#pragma once

#include <engine/public/gameObject.h>

class WeaponSwordPrefab final : public GameObject {
public:
    explicit WeaponSwordPrefab(Scene& scene);
    ~WeaponSwordPrefab() override = default;
};
