#pragma once

#include <game/prefabs/prefab_registrable.h>

class WeaponAxeConfig final : public PrefabRegistrable {
public:
    explicit WeaponAxeConfig();
    ~WeaponAxeConfig() override = default;

    void register_prefab(Scene& scene) override;
    
    constexpr static int health_amount = 15;
};
