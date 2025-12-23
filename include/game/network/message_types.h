#pragma once

#include <engine/network/network_message.h>

enum class CustomMessageTypes : uint16_t {
    USER_JOIN,
    USER_LEAVE,

    USER_MOVE,
};

struct MsgUserJoin {
    char uuid[37];
};

struct MsgUserLeave {
    char uuid[37];
};

struct MsgUserMove {
    char uuid[37];
    uint32_t size;
    uint8_t movement[4];
};
