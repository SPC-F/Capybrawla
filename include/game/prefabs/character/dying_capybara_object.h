#pragma once

#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class DyingCapybaraObject final : public GameObject {
public:
  explicit DyingCapybaraObject(Scene& scene, Vector3 position, float disappear_time = 2.0f);
  ~DyingCapybaraObject() override = default;

private:
    float disappear_time_;
    float elapsed_time_{0.0f};
};
