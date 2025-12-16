#include <game/scenes/swamp.h>

#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/physics/physics_service.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/layers.h>
#include <game/character/player_object.h>
#include <game/scenes/level_loader.h>

void load_players(Scene& scene, float start_x = 1000.0f, float start_y = 500.0f) {
    auto& player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x, start_y, 0});
    player.layer(Layers::Foreground);
}

Scene& SwampScene::setup() {
    const Engine& engine = Engine::instance();

    RenderingService& rendering_service = Engine::instance().services->get_service<RenderingService>().get();
    int window_width = rendering_service.window().get_window_width();
    int window_height = rendering_service.window().get_window_height();

    Scene& scene = engine.services->get_service<SceneService>().get().add_scene(SCENE_NAME);
    auto& camera = scene.add_game_object<Camera>(scene, Color(), 1.0f, true);
    camera.transform().position({window_width / 2.0f, window_height / 2.0f, 0.0f});
    
    LevelLoader loader;
    loader.load_game_objects_from_json(
        std::string(Assets::MAP_SWAMP),
        scene
    );

    load_players(scene);

    return scene;
}