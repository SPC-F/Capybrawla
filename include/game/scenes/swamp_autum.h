#pragma once

#include "../assets.h"

#include <engine/public/scene.h>

class SwampAutumScene {
public:
    static constexpr float out_of_bounds_margin_x = 480.0f;
    static constexpr float out_of_bounds_margin_y = 270.0f;
    static constexpr float map_width = 1920.0f;
    static constexpr float map_height = 1080.0f;

    static constexpr const char* SCENE_NAME = "Level_SwampAutumScene";
    static Scene& setup();
};