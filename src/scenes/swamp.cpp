#include <game/scenes/swamp.h>

#include <engine/physics/physics_service.h>
#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/layers.h>
#include <game/character/player_object.h>
#include <game/prefabs/ai_agent_object.h>
#include <game/scenes/level_loader.h>

void load_players(Scene& scene, float start_x = 1000.0f, float start_y = 500.0f) {
    auto& player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x, start_y, 0});
    player.layer(Layers::Foreground);
}

Scene& SwampScene::setup() {
    const Engine& engine = Engine::instance();
    Scene& scene = engine.services->get_service<SceneService>().get().add_scene(SCENE_NAME);
    auto& camera = scene.add_game_object<Camera>(scene, Color(), 1.0f, true);
    camera.transform().position({1920.0f / 2.0f, 1080.0f / 2.0f, 0.0f});
    
    auto& physics_service = engine.services->get_service<PhysicsService>().get();
    physics_service.debug_mode(true);

    LevelLoader loader;
    loader.load_game_objects_from_json(
        std::string(Assets::MAP_SWAMP),
        scene
    );

    load_players(scene);

    std::vector<Vector3> patrol_points = {
        Vector3{1400.0f, 300.0f, 0.0f},
        Vector3{500.0f, 400.0f, 0.0f},
    };
    auto& ai_agent = scene.add_game_object<AIAgentObject>(scene, Vector3{1000.0f, 600.0f, 0.0f}, patrol_points);

    return scene;
}