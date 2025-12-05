#pragma once

#include <string>
#include <vector>

struct LoadResource {
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

class Assets {
public:
    static constexpr const char* SWAMP_BACKGROUND = "swamp_background.png";
    static constexpr const char* SWAMP_COMPACT = "swamp_compact.png";
    static constexpr const char* MAP_SWAMP = "resources/maps/map_swamp.txt";
    
    static constexpr const char* SWAMP_AUTUM_BACKGROUND = "swamp_autum_background.png";
    static constexpr const char* SWAMP_AUTUM_COMPACT = "swamp_autum_compact.png";
    static constexpr const char* MAP_SWAMP_AUTUM = "resources/maps/map_swamp_autum.txt";

    static void load_resources(std::vector<LoadResource>& resources);
    static void register_textures(std::vector<LoadTexture>& textures);
};