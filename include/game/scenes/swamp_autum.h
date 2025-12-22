#pragma once

#include <game/assets.h>

#include <engine/public/scene.h>
#include <engine/network/multiplayer_service.h>

class SwampAutumScene {
public:
    static constexpr float out_of_bounds_margin_x = 480.0f;
    static constexpr float out_of_bounds_margin_y = 270.0f;
    static constexpr float map_width = 1920.0f;
    static constexpr float map_height = 1080.0f;

    static constexpr ConnectionState connection_state = ConnectionState::NONE;

    static constexpr const char* SCENE_NAME = "Level_SwampAutumScene";
    static Scene& setup();
};