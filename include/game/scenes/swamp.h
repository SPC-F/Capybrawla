#pragma once

#include "../assets.h"

#include <engine/public/scene.h>

class SwampScene {
public:
    static constexpr const char* SCENE_NAME = "Level_SwampScene";
    static Scene& setup();
};