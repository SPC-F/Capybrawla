#pragma once

#include <engine/public/gameObject.h>

class HealthPackPrefab final : public GameObject {
public:
    explicit HealthPackPrefab(Scene& scene);
    ~HealthPackPrefab() override = default;
};
