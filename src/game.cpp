#include "game.h"

#include <SDL3/SDL.h>

#include "assets.h"
#include "scenes/main_menu.h"
#include "scenes/swamp.h"
#include "scenes/swamp_autum.h"

#include <engine/core/engine.h>
#include <engine/core/rendering/assetService.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/scene_service.h>

void Game::initialize() {
    const Engine& engine = Engine::instance();
    Engine::initialize();

    std::vector<LoadResource> resources {
        // UI
        // Main menu
        {"ui/main_menu.png", "main_menu_bg", 1, 1},
        {"ui/buttons_large.png", "buttons_large", 2, 2},
        {"ui/buttons_big.png", "buttons_big", 4, 2},
        {"ui/buttons_small.png", "buttons_small", 3, 7},

        // Levels
        // Swamp resources
        {"swamp_background.png", "swamp_bg", 1, 1},
        {"swamp_compact.png", "swamp_tiles", 9, 12},

        // Swamp autum resources
        {"swamp_autum_background.png", "swamp_autum_bg", 1, 1},
        {"swamp_autum_compact.png", "swamp_autum_tiles", 9, 12}
    };
    Assets::load_resources(resources);

    std::vector<LoadTexture> textures {
        // UI textures
        {"buttons_large", "button_large_blue", 0},
        {"buttons_large", "button_large_red", 1},
        {"buttons_large", "button_large_yellow", 2},
        {"buttons_large", "button_large_green", 3},

        {"buttons_big", "button_big_blue", 0},
        {"buttons_big", "button_big_red", 1},
        {"buttons_big", "button_big_yellow", 2},
        {"buttons_big", "button_big_green", 3},
        {"buttons_big", "button_big_black", 4},
        {"buttons_big", "button_big_transparent", 5},
        {"buttons_big", "button_big_hollow", 6},

        {"buttons_small", "button_small_blue", 0},
        {"buttons_small", "button_small_red", 1},
        {"buttons_small", "button_small_black_round", 2},
        {"buttons_small", "button_small_red_round", 3},
        {"buttons_small", "button_small_green_round", 4},
        {"buttons_small", "button_small_arrow", 5},
        {"buttons_small", "button_small_arrow_double", 6},
        {"buttons_small", "button_small_yellow", 7},
        {"buttons_small", "button_small_green", 8},
        {"buttons_small", "button_small_black_deny", 9},
        {"buttons_small", "button_small_black_allow", 10},
        {"buttons_small", "button_small_deny", 11},
        {"buttons_small", "button_small_allow", 12},
        {"buttons_small", "button_small_transparent", 14},
        {"buttons_small", "button_small_black_arrow_left", 16},
        {"buttons_small", "button_small_black_arrow_right", 17},
        {"buttons_small", "button_small_black_deny", 18},
        {"buttons_small", "button_small_black_accept", 19},

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

    MainMenuScene main_menu;
    Scene& main_menu_scene = main_menu.setup(
        []() {},
        [](const std::string& address) {
            SwampAutumScene swamp_autum;
            auto& swamp_autum_scene = swamp_autum.setup();
            Engine::instance().services->get_service<SceneService>().get().load_scene(swamp_autum_scene.name());
        },
        []() {
            SwampScene swamp;
            auto& swamp_scene = swamp.setup();
            Engine::instance().services->get_service<SceneService>().get().load_scene(swamp_scene.name());
        }
    );

    scene_service.load_scene(main_menu_scene.name());
}

void Game::shutdown() {
    Engine& engine = Engine::instance();
    engine.quit();
}