#pragma once

#include <game/character/player_object.h>
#include <engine/public/gameObject.h>

namespace PlayerInfoComponent {
  GameObject& create_and_add(Scene &scene, PlayerObject &player);
};