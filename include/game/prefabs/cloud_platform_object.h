#pragma once

#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class CloudPlatformObject final : public GameObject {
public:
  explicit CloudPlatformObject(Scene& scene, const Vector3 position);
  ~CloudPlatformObject() override = default;

};
