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

    LOBBY_DATA,
    ROUND_START,
    ROUND_END
};

struct MsgUserJoin {
    char uuid[37];
};

struct MsgUserLeave {
    char uuid[37];
};

struct LobbyUserData {
    char uuid[37];
    uint16_t color;
    char name[255];
};

struct MsgLobbyData {
    uint32_t user_count;
    LobbyUserData users[4];  // Max 4 players
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

struct MsgRoundStart {
    // Don't need any data to send
};

struct MsgRoundEnd {
    char winner_uuid[37];
    bool draw;
};