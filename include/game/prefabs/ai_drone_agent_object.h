#pragma once

#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class AIDroneAgentObject final : public GameObject {
public:
  explicit AIDroneAgentObject(Scene& scene, const Vector3 initial_pos, std::vector<Vector3> target_positions);
  ~AIDroneAgentObject() override = default;

private:
 std::vector<Vector3> patrol_points_;
 size_t location_index_{0};
 std::optional<std::reference_wrapper<GameObject>> patrol_target_{std::nullopt};
};
