#include <game/game.h>

#include <game/assets.h>
#include <game/scenes/main_menu.h>
#include <game/scenes/swamp.h>
#include <game/scenes/swamp_autum.h>

#include <engine/core/engine.h>
#include <engine/core/rendering/assetService.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/scene_service.h>

#include "engine/public/components/behaviorscript.h"
#include "game/pause_menu_ui.h"
#include "game/behaviors/pause_play_behavior.h"

namespace
{
    void strap_pause_menu(Scene& main_menu_scene)
    {
        auto ppmenu = std::make_unique<PauseMenuUI>(main_menu_scene);
        ppmenu->mark_dont_destroy_on_load(true);

        // Get the ID for the pause menu
        const auto ppmId = ppmenu->id();

        // Create and configure the Pause/Play controller
        auto& ppc = main_menu_scene.add_game_object("Pause/Play controller");
        ppc.add_component<BehaviorScript>(std::make_unique<PausePlayBehavior>(*ppmenu));
        ppc.mark_dont_destroy_on_load(true);

        // Add the PauseMenuUI object to the scene
        main_menu_scene.add_game_object(std::move(ppmenu));
    }
}

void Game::initialize() {
    const Engine& engine = Engine::instance();
    Engine::initialize();

    std::vector<LoadResource> resources {
        // UI
        // Main menu
        {"ui/main_menu.png", "main_menu_bg", 1, 1},
        {"ui/buttons_large.png", "buttons_large", 2, 2},
        {"ui/buttons_big.png", "buttons_big", 4, 2},
        {"ui/buttons_small.png", "buttons_small", 4, 5},

        // Levels
        // Swamp resources
        {"background/swamp/swamp_background.png", "swamp_bg", 1, 1},
        {"tilemaps/swamp_compact.png", "swamp_tiles", 9, 12},

        // Swamp autum resources
        {"background/swamp_autum/swamp_autum_background.png", "swamp_autum_bg", 1, 1},
        {"tilemaps/swamp_autum_compact.png", "swamp_autum_tiles", 9, 12},

        // Characters
        {"character/capybara_default_idle.png", "capybara_default_idle", 1, 1},
        {"character/capybara_red_idle.png", "capybara_red_idle", 1, 1},
        {"character/capybara_blue_idle.png", "capybara_blue_idle", 1, 1},
        {"character/capybara_green_idle.png", "capybara_green_idle", 1, 1}

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
        {"buttons_big", "button_big_transparent", 4},
        {"buttons_big", "button_big_hollow", 5},
        {"buttons_big", "button_big_black", 6},

        {"buttons_small", "button_small_blue", 0},
        {"buttons_small", "button_small_red", 1},
        {"buttons_small", "button_small_yellow", 2},
        {"buttons_small", "button_small_green", 3},
        {"buttons_small", "button_small_black", 4},
        {"buttons_small", "button_small_blue_round", 5},
        {"buttons_small", "button_small_red_round", 6},
        {"buttons_small", "button_small_yellow_round", 7},
        {"buttons_small", "button_small_green_round", 8},
        {"buttons_small", "button_small_black_round", 9},
        {"buttons_small", "button_small_hollow", 10},
        {"buttons_small", "button_small_black_deny", 11},
        {"buttons_small", "button_small_transparent", 12},
        {"buttons_small", "button_small_deny", 13},
        {"buttons_small", "button_small_allow", 14},
        {"buttons_small", "button_small_black_empty", 15},
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

    strap_pause_menu(main_menu_scene);

    scene_service.load_scene(main_menu_scene.name());
}

void Game::shutdown() {
    Engine& engine = Engine::instance();
    engine.quit();
}