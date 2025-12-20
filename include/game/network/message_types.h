#pragma once

#include <engine/network/network_message.h>

enum class CustomMessageTypes : uint16_t {
    USER_JOIN,
    USER_LEAVE,
};

struct MsgUserJoin {
    char uuid[37];
};

struct MsgUserLeave {
    char uuid[37];
};
