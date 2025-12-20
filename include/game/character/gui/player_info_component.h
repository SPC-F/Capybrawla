#pragma once

#include <engine/public/gameObject.h>
#include <game/character/player_object.h>

namespace PlayerInfoComponent {
    GameObject &create_and_add(Scene &scene, PlayerObject &player);
};
