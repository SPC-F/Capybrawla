#pragma once

#include <game/prefabs/prefab_registrable.h>

class WeaponSwordConfig final : public PrefabRegistrable {
public:
    explicit WeaponSwordConfig();
    ~WeaponSwordConfig() override = default;

    void register_prefab(Scene& scene) override;
    
    constexpr static int health_amount = 15;
};
