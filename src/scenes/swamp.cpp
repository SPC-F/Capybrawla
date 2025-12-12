#include "engine/physics/physics_service.h"

#include <engine/core/engine.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/scene_service.h>
#include <engine/public/util/layers.h>
#include <game/character/player_object.h>
#include <game/scenes/level_loader.h>
#include <game/scenes/swamp.h>

void load_level_background(Scene& scene) {
    GameObject& bg = scene.add_game_object("Background");
    bg.add_component<Sprite>("swamp_bg", Color{255, 255, 255, 255}, 0, 0, 0, 0);
    bg.transform().position({0, 0, 0});
    bg.transform().scale({1, 1, 1});

    std::unordered_map<char, std::string> texture_map {
            {'+', "grass_single"},

            {'<', "grass_multi_top_left"},
            {'#', "grass_multi_top"},
            {'>', "grass_multi_top_right"},

            {'[', "grass_multi_middle_left"},
            {'0', "grass_multi_middle"},
            {']', "grass_multi_middle_right"},

            {'\\', "grass_multi_bottom_left"},
            {'/', "grass_multi_bottom_right"},
            {'-', "grass_multi_bottom"}
    };

    LevelLoader::load_game_objects_from_file(
        std::string(Assets::MAP_SWAMP),
        scene,
        texture_map
    );
}

void load_players(Scene& scene) {
    auto& player = scene.add_game_object<PlayerObject>(scene, Vector3{500, 500, 0});
    player.layer(Layers::Foreground);
}

Scene& SwampScene::setup() {
    const std::string swamp_scene_tag = "Level_SwampScene";

    const Engine& engine = Engine::instance();
    Scene& scene = engine.services->get_service<SceneService>().get().add_scene(swamp_scene_tag);
    PhysicsService& physics_service = engine.services->get_service<PhysicsService>().get();
    scene.add_game_object<Camera>(scene, Color(), 1.0f, true);
    
    load_level_background(scene);
    load_players(scene);

    return scene;
}