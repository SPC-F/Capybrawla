#pragma once

#include <cstdint>

enum class PlayerMovementTypes : uint8_t {
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP,
    CROUCH,
};