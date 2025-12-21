#pragma once
#include "engine/public/gameObject.h"

class HealthPackPrefab final : public GameObject {
private:
    constexpr static int health_amount_ = 15;

public:
    explicit HealthPackPrefab(Scene& scene);
    ~HealthPackPrefab() override = default;

    [[nodiscard]] std::unique_ptr<GameObject> clone() const override
    {
        return std::make_unique<HealthPackPrefab>(scene());
    }
};
