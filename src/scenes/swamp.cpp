#include "engine/physics/physics_service.h"

#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/layers.h>
#include <game/character/player_object.h>
#include <game/scenes/level_loader.h>
#include <game/scenes/swamp.h>

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
    
    LevelLoader loader;
    loader.load_game_objects_from_json(
        std::string(Assets::MAP_SWAMP),
        scene
    );

    load_players(scene);
    physics_service.debug_mode(true);

    return scene;
}