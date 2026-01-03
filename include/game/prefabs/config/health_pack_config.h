#pragma once

#include <game/prefabs/prefab_registrable.h>

class HealthPackConfig final : public PrefabRegistrable {
public:
    explicit HealthPackConfig();
    ~HealthPackConfig() override = default;

    void register_prefab(Scene& scene) override;
    
    constexpr static int health_amount = 15;
};
