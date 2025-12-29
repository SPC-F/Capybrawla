#pragma once
#include "engine/public/gameObject.h"

class HealthPackPrefab final : public GameObject {
private:
    constexpr static int health_amount_ = 15;

public:
    explicit HealthPackPrefab(Scene& scene);
    ~HealthPackPrefab() override = default;
};
