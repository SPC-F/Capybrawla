#pragma once

#include <game/prefabs/prefab_registrable.h>

class WeaponBatConfig final : public PrefabRegistrable {
public:
    explicit WeaponBatConfig();
    ~WeaponBatConfig() override = default;

    void register_prefab(Scene& scene) override;
    
    constexpr static int health_amount = 15;
};
