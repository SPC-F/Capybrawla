#include <game/scenes/level_loader.h>

#include <fstream>
#include <stdexcept>

#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/color.h>

#include <game/util/json.hpp>

constexpr int DEFAULT_TILE_SIZE = 16;

using json = nlohmann::json;

void LevelLoader::load_game_objects_from_json(
    const std::string& file_path, 
    Scene& scene
) {
    std::ifstream map_file(file_path);
    if (!map_file.is_open()) {
        throw std::runtime_error("Failed to open map file: " + file_path);
    }

    json map_json;
    map_file >> map_json;

    int tile_size = map_json.value("cell_size", DEFAULT_TILE_SIZE);
    int tile_x_center_offset = tile_size * 18;
    int tile_y_center_offset = tile_size * 8;

    if (map_json.contains("backgrounds")) {
        for (const auto& background_json : map_json["backgrounds"]) {
            GameObject& bg = scene.add_game_object("background_" + background_json.value("name", "default"));
            std::string bg_texture = background_json.value("texture", "");

            auto pos = background_json["position"];
            int x = pos.value("x", 0);
            int y = pos.value("y", 0);
            
            auto scale = background_json["scale"];
            float scale_x = scale.value("x", 1.0f);
            float scale_y = scale.value("y", 1.0f);
            
            int layer = background_json.value("layer", 0);

            bg.add_component<Sprite>(
                bg_texture, 
                Color{255,255,255,255}, 
                0,0,0,0
            );
            bg.transform().position({static_cast<float>(x), static_cast<float>(y), 0.0f});
            bg.transform().scale({scale_x, scale_y, 1.0f});
            bg.layer(layer);
        }
    }

    if (map_json.contains("tiles")) {
        for (const auto& tile_json : map_json["tiles"]) {

            std::string texture_name =
                tile_json.value("texture", tile_json.value("tile", ""));

            auto pos = tile_json.value("position", json::object());
            int grid_x = pos.value("x", 0);
            int grid_y = pos.value("y", 0);

            auto props = tile_json["properties"];
            auto size = props["size"];
            int local_tile_h = size.value("h", tile_size);
            int local_tile_w = size.value("w", tile_size);

            auto scale = tile_json.value("scale", json::object());
            float scale_x = scale.value("x", 1.0f);
            float scale_y = scale.value("y", 1.0f);

            int layer = tile_json.value("layer", 1);

            float world_x = grid_x * local_tile_w * scale_x;
            float world_y = grid_y * local_tile_h * scale_y;

            GameObject& tile_obj = scene.add_game_object(
                "Tile_" + std::to_string(grid_x) + "_" + std::to_string(grid_y)
            );

            tile_obj.add_component<Sprite>(
                texture_name,
                Color{255,255,255,255},
                0,0,0,0
            );

            tile_obj.transform().position({ world_x + tile_x_center_offset, world_y + tile_y_center_offset, 0.0f });
            tile_obj.transform().scale({ scale_x, scale_y, 1.0f });
            tile_obj.layer(layer);
        }
    }
}