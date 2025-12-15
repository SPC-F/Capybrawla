#pragma once

#include <vector>
#include <unordered_map>

#include <engine/public/scene.h>

struct LevelTile {
    char symbol;
    int x;
    int y;
};

class LevelLoader {
public:
    static void load_game_objects_from_json(
        const std::string& file_path, 
        Scene& scene
    );
};
