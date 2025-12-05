#include <game/game.h>

#include <game/assets.h>
#include <game/scenes/swamp.h>
#include <game/scenes/swamp_autum.h>

#include <engine/core/engine.h>
#include <engine/core/rendering/assetService.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/scene_service.h>

void Game::initialize() {
    const Engine& engine = Engine::instance();
    Engine::initialize();

    std::vector<LoadResource> resources {
        // Swamp resources
        {"swamp_background.png", "swamp_bg", 1, 1},
        {"swamp_compact.png", "swamp_tiles", 9, 12},

        // Swamp autum resources
        {"swamp_autum_background.png", "swamp_autum_bg", 1, 1},
        {"swamp_autum_compact.png", "swamp_autum_tiles", 9, 12}
    };
    Assets::load_resources(resources);

    std::vector<LoadTexture> textures {
        // Swamp textures
        {"swamp_tiles", "grass_single", 24},

        {"swamp_tiles", "grass_multi_top_left", 1},
        {"swamp_tiles", "grass_multi_top", 2},
        {"swamp_tiles", "grass_multi_top_right", 3},

        {"swamp_tiles", "grass_multi_middle_left", 13},
        {"swamp_tiles", "grass_multi_middle", 14},
        {"swamp_tiles", "grass_multi_middle_right", 15},

        {"swamp_tiles", "grass_multi_bottom_left", 25},
        {"swamp_tiles", "grass_multi_bottom", 26},
        {"swamp_tiles", "grass_multi_bottom_right", 27},

        // Swamp autum textures
        {"swamp_autum_tiles", "grass_autum_single", 24},

        {"swamp_autum_tiles", "grass_autum_multi_top_left", 1},
        {"swamp_autum_tiles", "grass_autum_multi_top", 2},
        {"swamp_autum_tiles", "grass_autum_multi_top_right", 3},

        {"swamp_autum_tiles", "grass_autum_multi_middle_left", 13},
        {"swamp_autum_tiles", "grass_autum_multi_middle", 14},
        {"swamp_autum_tiles", "grass_autum_multi_middle_right", 15},

        {"swamp_autum_tiles", "grass_autum_multi_bottom_left", 25},
        {"swamp_autum_tiles", "grass_autum_multi_bottom", 26},
        {"swamp_autum_tiles", "grass_autum_multi_bottom_right", 27}
    };
    Assets::register_textures(textures);

    auto& window_controller = engine.services->get_service<RenderingService>().get().window();
    window_controller.set_window_fullscreen();
}

void Game::run() {
    Engine& engine = Engine::instance();
    auto& scene_service = engine.services->get_service<SceneService>().get();
    
    Scene& scene = SwampScene::setup();

    scene_service.load_scene(scene.name());
}

void Game::shutdown() {
    Engine& engine = Engine::instance();
    engine.quit();
}