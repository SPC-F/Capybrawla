#pragma once

#include <engine/network/network_message.h>

enum class CustomMessageTypes : uint16_t {
    USER_JOIN,
    USER_LEAVE,

    USER_MOVE,
    USER_ATTACK,
    USER_RESPAWN,
    USER_DROP_WEAPON,
    DROP_SPAWN,
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

struct MsgUserAttack {
    char uuid[37];
};

struct MsgDropSpawn {
    char spawner_uuid[37];
    char drop_uuid[37];
    char drop_type[255];
};

struct MsgUserRespawn {
    char uuid[37];
    float x;
    float y;
    float z; 
};

struct MsgUserDropWeapon {
    char uuid[37];
};
