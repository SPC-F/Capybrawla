#pragma once

#include <engine/public/gameObject.h>

class WeaponBatPrefab final : public GameObject {
public:
    explicit WeaponBatPrefab(Scene& scene);
    ~WeaponBatPrefab() override = default;
};
