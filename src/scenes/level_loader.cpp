#include "level_loader.h"

#include <fstream>
#include <stdexcept>

#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/color.h>

void LevelLoader::load_game_objects_from_file(
    const std::string& file_path, 
    Scene& scene,
    std::unordered_map<char, std::string> texture_map
) {
    std::ifstream map_file(file_path);
    if (!map_file.is_open()) {
        throw std::runtime_error("Failed to open map file: " + file_path);
    }

    int tile_size = 16;
    int width_pixels = 0;
    int height_pixels = 0;

    std::string line;

    std::getline(map_file, line);
    tile_size = std::stoi(line);

    std::getline(map_file, line);
    width_pixels = std::stoi(line);

    std::getline(map_file, line);
    height_pixels = std::stoi(line);

    std::getline(map_file, line); // skip empty line

    std::vector<LevelTile> tiles;

    int y = 0;
    while (std::getline(map_file, line)) {
        for (int x = 0; x < static_cast<int>(line.length()); ++x) {
            char c = line[x];

            if (c == 'O') continue; // empty tile

            LevelTile t;
            t.symbol = c;
            t.x = x * tile_size;
            t.y = y * tile_size;
            tiles.push_back(t);
        }
        ++y;
    }

    for (const auto& tile : tiles) {
        for (const auto& [symbol, texture_name] : texture_map) {
            if (tile.symbol != symbol) {
                continue;    
            }

            GameObject& tile_obj = scene.add_game_object("Tile_" + std::to_string(tile.x) + "_" + std::to_string(tile.y));
            tile_obj.add_component<Sprite>(texture_name, Color{255, 255, 255, 255}, 0, 0, 0, 0);
            tile_obj.transform().position({static_cast<float>(tile.x), static_cast<float>(tile.y), 0.0f});
            tile_obj.transform().scale({2, 2, 2});
        }
    }
}