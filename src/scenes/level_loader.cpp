
#include <game/scenes/level_loader.h>

#include <fstream>
#include <stdexcept>

#include <engine/core/engine.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/ai/ai_controller.h>
#include <engine/public/components/ai/navigation/navigation_graph.h>
#include <engine/public/components/ai/navigation/pathfinding.h>
#include <engine/public/gameObject.h>
#include <engine/public/scene_service.h>
#include <engine/public/util/color.h>

constexpr int DEFAULT_TILE_SIZE = 16;

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

    tile_size_ = map_json.value("cell_size", DEFAULT_TILE_SIZE);
    tile_x_center_offset_ = tile_size_ * map_json.value("center_x_offset", 0);
    tile_y_center_offset_ = tile_size_ * map_json.value("center_y_offset", 0);

    bool navgraph_enabled = map_json.value("navgraph_enabled", false);
    bool pathfinding_enabled = map_json.value("pathfinding_enabled", false);

    if (map_json.contains("backgrounds")) {
        create_backgrounds(scene, map_json["backgrounds"]);
    }

    if (map_json.contains("tiles")) {
        create_tiles(scene, map_json["tiles"], navgraph_enabled, pathfinding_enabled);
        create_tile_colliders(scene, map_json["tiles"]);
    }
}

void LevelLoader::create_backgrounds(Scene& scene, const json& backgrounds_json) {
    for (const auto& background_json : backgrounds_json) {
        GameObject& bg = scene.add_game_object(
            "background_" + background_json.value("name", "default")
        );

        bg.add_component<Sprite>(
            background_json.value("texture", ""),
            Color{100,100,100,255},
            0,0,0,0
        );

        auto pos = background_json["position"];
        auto scale = background_json["scale"];

        bg.transform().position({
            static_cast<float>(pos.value("x", 0)),
            static_cast<float>(pos.value("y", 0)),
            0.0f
        });

        bg.transform().scale({
            scale.value("x", 1.0f),
            scale.value("y", 1.0f),
            1.0f
        });

        bg.layer(background_json.value("layer", 0));
    }
}

void LevelLoader::create_tiles(Scene& scene, const json& tiles_json, bool navgraph_enabled, bool pathfinding_enabled) {
    auto& dynamic_parent = scene.add_game_object("TileMapDynamic_Parent");
    auto& kinetic_parent = scene.add_game_object("TileMapKinematic_Parent");

    int grid_tile_size = 0;

    for (const auto& tile_json : tiles_json) {

        std::string texture_name =
            tile_json.value("texture", tile_json.value("tile", ""));

        auto pos = tile_json["position"];
        int grid_x = pos.value("x", 0);
        int grid_y = pos.value("y", 0);

        auto props = tile_json["properties"];
        auto size = props["size"];
        int local_tile_w = size.value("w", tile_size_);
        int local_tile_h = size.value("h", tile_size_);

        auto scale = tile_json.value("scale", json::object());
        float scale_x = scale.value("x", 1.0f);
        float scale_y = scale.value("y", 1.0f);

        if (grid_tile_size == 0) grid_tile_size = local_tile_w * scale_x;

        int layer = tile_json.value("layer", 1);
        int group = tile_json.value("group", 0);

        float world_x = grid_x * local_tile_w * scale_x + tile_x_center_offset_;
        float world_y = grid_y * local_tile_h * scale_y + tile_y_center_offset_;

        GameObject& tile_obj = scene.add_game_object(
            "Tile_" + std::to_string(grid_x) + "_" + std::to_string(grid_y)
        );

        tile_obj.add_component<Sprite>(
            texture_name,
            Color{255,255,255,255},
            0,0,0,0
        );

        tile_obj.transform().position({ world_x, world_y, 0.0f });
        tile_obj.transform().scale({ scale_x, scale_y, 1.0f });
        tile_obj.layer(layer);

        if (props.contains("collider")) {
            tile_obj.parent(dynamic_parent);

            auto collider_props = props["collider"];

            std::string type = collider_props.value("type", "");
            float collider_w = collider_props.value("width", local_tile_w);
            float collider_h = collider_props.value("height", local_tile_h);
            float offset_x = collider_props.value("offset_x", 0.0f);
            float offset_y = collider_props.value("offset_y", 0.0f);

            TileColliderData data;
            data.type = type;
            data.grid_x = grid_x;
            data.grid_y = grid_y;
            data.world_x = world_x;
            data.world_y = world_y;
            data.width  = collider_w * scale_x;
            data.height = collider_h * scale_y;
            data.offset_x = offset_x * scale_x;
            data.offset_y = offset_y * scale_y;
            data.group  = group;
            data.layer  = layer + 1;

            collider_rows[group][grid_y].push_back(data);
        }
        else {
            tile_obj.parent(kinetic_parent);
        }
    }

    if (navgraph_enabled) {
        auto& nav_graph = dynamic_parent.add_component<NavigationGraph>(grid_tile_size, 5);

        if (pathfinding_enabled) {
            dynamic_parent.add_component<Pathfinding>();
        }

        nav_graph.generate_graph();
    }
}

void LevelLoader::create_tile_colliders(Scene& scene, const json& tiles_json) {
    for (auto& [group, rows] : collider_rows) {
        for (auto& [row, tiles] : rows) {

            std::sort(tiles.begin(), tiles.end(),
                [](const TileColliderData& a, const TileColliderData& b) {
                    return a.grid_x < b.grid_x;
                });

            int run_start = 0;

            for (int i = 1; i <= static_cast<int>(tiles.size()); ++i) {
                bool end_run =
                    i == tiles.size() ||
                    tiles[i].grid_x != tiles[i - 1].grid_x + 1;

                if (!end_run) continue;

                const auto& first = tiles[run_start];
                const auto& last  = tiles[i - 1];

                int tile_count = last.grid_x - first.grid_x + 1;
                float merged_width = tile_count * first.width;

                /// Only box colliders are supported for merging
                if (first.type != "box") continue;

                GameObject& collider_obj = scene.add_game_object(
                    "MergedCollider_G" + std::to_string(group) +
                    "_Y" + std::to_string(row)
                );
                collider_obj.transform().position({
                    first.world_x,
                    first.world_y,
                    0.0f
                });
                collider_obj.add_component<Rigidbody2D>(
                    BodyType2D::Static,
                    1.0f,
                    false,
                    1.0f
                );
                collider_obj.add_component<BoxCollider2D>(
                    0.5f,
                    0.0f,
                    merged_width,
                    first.height,
                    Point{first.offset_x, first.offset_y}
                );
                collider_obj.layer(first.layer);

                run_start = i;
            }
        }
    }
}