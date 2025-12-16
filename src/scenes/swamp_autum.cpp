#include <game/scenes/swamp_autum.h>

#include <game/scenes/level_loader.h>

#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/sprite.h>

Scene& SwampAutumScene::setup() {
    Engine& engine = Engine::instance();
    Scene& scene = engine.services->get_service<SceneService>().get().add_scene(SCENE_NAME);
    
    LevelLoader loader;
    loader.load_game_objects_from_json(
        std::string(Assets::MAP_SWAMP_AUTUM),
        scene
    );

    return scene;
}