#include <game/scenes/swamp.h>

#include <game/scenes/level_loader.h>

#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/sprite.h>

Scene& SwampScene::setup() {
    const std::string swamp_scene_tag = "Level_SwampScene";

    const Engine& engine = Engine::instance();
    Scene& scene = engine.services->get_service<SceneService>().get().add_scene(swamp_scene_tag);
    scene.add_game_object<Camera>(scene, Color(), 1.0f, true);
    
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

    return scene;
}