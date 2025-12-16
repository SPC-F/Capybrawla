#pragma once

#include <vector>
#include <unordered_map>

#include <engine/public/scene.h>

#include <game/util/json.hpp>

using json = nlohmann::json;

/// Data structure to hold tile collider properties
struct TileColliderData {
    std::string type;
    int grid_x;
    int grid_y;
    float world_x;
    float world_y;
    float width;
    float height;
    float offset_x;
    float offset_y;
    int group;
    int layer;
};

class LevelLoader {
public:
    void load_game_objects_from_json(
        const std::string& file_path, 
        Scene& scene
    );

private:
    int tile_size_ = 16;
    int tile_x_center_offset_ = 0;
    int tile_y_center_offset_ = 0;
    std::unordered_map<int, std::unordered_map<int, std::vector<TileColliderData>>> collider_rows;

    void create_backgrounds(Scene& scene, const json& backgrounds_json);
    void create_tiles(Scene& scene, const json& tiles_json);
    void create_tile_colliders(Scene& scene, const json& tiles_json);
};
