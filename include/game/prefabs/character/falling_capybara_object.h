#pragma once

#include <engine/public/gameObject.h>
#include <engine/public/util/vector3.h>

class FallingCapybaraObject final : public GameObject {
public:
  explicit FallingCapybaraObject(Scene& scene, float max_fall_height, float size_modifier, float rotation_speed, float fall_speed, float position_x, float position_y);
  ~FallingCapybaraObject() override = default;

    [[nodiscard]] std::unique_ptr<GameObject> clone() const override
    {
        return std::make_unique<FallingCapybaraObject>(scene(), max_fall_height_, size, rotation_speed, fall_speed, position_x, position_y);
    }

private:
    float max_fall_height_{800.0f};
    float size{2.0f};
    float rotation_speed{20.0f};
    float fall_speed{50.0f};
    float position_x{0.0f};
    float position_y{0.0f};
    
    std::vector<std::string> capybara_variants_{
        "capybara_default_idle",
        "capybara_red_idle",
        "capybara_blue_idle",
        "capybara_green_idle",
    };
};
