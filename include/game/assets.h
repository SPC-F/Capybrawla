#pragma once

#include <string>
#include <vector>
#include <engine/audio/sound/sound_type.h>

struct LoadResourceData {
    std::string file;
    std::string name;
    int rows;
    int columns;
};

struct LoadTexture {
    std::string asset_name;
    std::string texture_name;
    int index;
};

struct LoadAnimation {
    std::string resource_name;
    std::string animation_name;
    int start_frame;
    int frame_count;
};

struct LoadAudio {
    std::string path;
    std::string name;
    SoundType type;
};

class Assets {
public:
    static constexpr const char* SWAMP_BACKGROUND = "swamp_background.png";
    static constexpr const char* SWAMP_COMPACT = "swamp_compact.png";
    static constexpr const char* MAP_SWAMP = "resources/maps/map_swamp.json";
    
    static constexpr const char* SWAMP_AUTUM_BACKGROUND = "swamp_autum_background.png";
    static constexpr const char* SWAMP_AUTUM_COMPACT = "swamp_autum_compact.png";
    static constexpr const char* MAP_SWAMP_AUTUM = "resources/maps/map_swamp_autum.json";

    static void load_resources(const std::vector<LoadResourceData>& resources);
    static void register_textures(const std::vector<LoadTexture>& textures);
    static void register_sprite_sheets(const std::vector<LoadAnimation>& animations);
    static void register_audio(const std::vector<LoadAudio>& audio);
};