#include <game/scenes/swamp.h>

#include <game/scenes/level_loader.h>

#include <engine/core/engine.h>
#include <engine/physics/physics_service.h>
#include <engine/public/scene_service.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/sprite.h>

Scene& SwampScene::setup() {
    const std::string swamp_scene_tag = "Level_SwampScene";

    const Engine& engine = Engine::instance();
    Scene& scene = engine.services->get_service<SceneService>().get().add_scene(swamp_scene_tag);
    scene.add_game_object<Camera>(scene, Color(), 1.0f, true);

    LevelLoader loader;
    loader.load_game_objects_from_json(
        std::string(Assets::MAP_SWAMP),
        scene
    );

    return scene;
}