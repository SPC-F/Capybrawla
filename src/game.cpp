#include <game/game.h>
#include <game/assets.h>
#include <game/scenes/main_menu.h>
#include <game/scenes/swamp.h>
#include <game/scenes/swamp_autum.h>

#include <engine/core/engine.h>
#include <engine/core/rendering/assetService.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/scene_service.h>

#include <game/pause_menu_ui.h>
#include <game/behaviors/pause_play_behavior.h>

namespace
{
    void strap_pause_menu(Scene& main_menu_scene)
    {
        auto ppmenu = std::make_unique<PauseMenuUI>(main_menu_scene);
        ppmenu->mark_dont_destroy_on_load(true);

        auto& ppc = main_menu_scene.add_game_object("Pause/Play controller");
        ppc.add_component<BehaviorScript>(std::make_unique<PausePlayBehavior>(*ppmenu));
        ppc.mark_dont_destroy_on_load(true);

        main_menu_scene.add_game_object(std::move(ppmenu));
    }
}

void Game::initialize() {
    const Engine& engine = Engine::instance();
    Engine::initialize();

    const std::vector<LoadResource> resources {
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
        {"character/capybara_default_duck.png", "capybara_default_duck", 1, 1},
        {"character/capybara_default_walk_anim.png", "capybara_default_walk_anim_sheet", 1, 8},
        {"character/capybara_default_idle_anim.png", "capybara_default_idle_anim_sheet", 1, 7},
        {"character/capybara_default_jump_anim.png", "capybara_default_jump_anim_sheet", 1, 7},
        {"character/capybara_default_duck_anim.png", "capybara_default_duck_anim_sheet", 1, 7},

        {"character/capybara_red_idle.png", "capybara_red_idle", 1, 1},
        {"character/capybara_blue_idle.png", "capybara_blue_idle", 1, 1},
        {"character/capybara_green_idle.png", "capybara_green_idle", 1, 1},

        {"character/drone_idle.png", "drone_idle", 1, 1},
        {"character/drone_idle_anim.png", "drone_idle_anim_sheet", 1, 7},

        // Weapons
        {"weapons/bat.png", "bat_sheet", 1, 2},
    };
    Assets::load_resources(resources);

    const std::vector<LoadTexture> textures {
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
        {"swamp_tiles", "grass_single_top", 0},
        {"swamp_tiles", "grass_single_bottom", 12},

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

        {"swamp_tiles", "grass_slope_top_left", 37},
        {"swamp_tiles", "grass_slope_top_right", 39},
        {"swamp_tiles", "grass_slope_bottom_left", 49},
        {"swamp_tiles", "grass_slope_bottom_right", 51},

        {"swamp_tiles", "rock_single", 19},

        {"swamp_tiles", "rock_single_top", 43},
        {"swamp_tiles", "rock_single_bottom", 55},

        {"swamp_tiles", "rock_multi_top_left", 37},
        {"swamp_tiles", "rock_multi_top", 38},
        {"swamp_tiles", "rock_multi_top_right", 39},

        {"swamp_tiles", "rock_multi_middle_left", 50},
        {"swamp_tiles", "rock_multi_middle", 51},
        {"swamp_tiles", "rock_multi_middle_right", 52},

        {"swamp_tiles", "rock_multi_bottom_left", 62},
        {"swamp_tiles", "rock_multi_bottom", 63},
        {"swamp_tiles", "rock_multi_bottom_right", 64},

        {"swamp_tiles", "wood_walk_piece", 86},

        {"swamp_tiles", "grass", 9},
        {"swamp_tiles", "bush_small", 8},
        {"swamp_tiles", "bush_large", 33},
        {"swamp_tiles", "lilypad_large", 32},

        {"swamp_tiles", "fence_left", 56},
        {"swamp_tiles", "fence_right", 57},
        {"swamp_tiles", "fence", 58},
        
        {"swamp_tiles", "rock_small", 77},
        {"swamp_tiles", "rock_large", 76},

        // Swamp autum textures
        {"swamp_autum_tiles", "grass_autum_single_top", 0},
        {"swamp_autum_tiles", "grass_autum_single_bottom", 12},

        {"swamp_autum_tiles", "grass_autum_single", 24},
        
        {"swamp_autum_tiles", "grass_autum_multi_top_left", 1},
        {"swamp_autum_tiles", "grass_autum_multi_top", 2},
        {"swamp_autum_tiles", "grass_autum_multi_top_right", 3},

        {"swamp_autum_tiles", "grass_autum_multi_middle_left", 13},
        {"swamp_autum_tiles", "grass_autum_multi_middle", 14},
        {"swamp_autum_tiles", "grass_autum_multi_middle_right", 15},
        {"swamp_autum_tiles", "grass_autum_multi_bottom_left", 25},
        {"swamp_autum_tiles", "grass_autum_multi_bottom", 26},
        {"swamp_autum_tiles", "grass_autum_multi_bottom_right", 27},

        {"swamp_autum_tiles", "grass_autum_slope_top_left", 37},
        {"swamp_autum_tiles", "grass_autum_slope_top_right", 39},
        {"swamp_autum_tiles", "grass_autum_slope_bottom_left", 49},
        {"swamp_autum_tiles", "grass_autum_slope_bottom_right", 51},

        {"swamp_autum_tiles", "rock_single", 19},

        {"swamp_autum_tiles", "rock_single_top", 43},
        {"swamp_autum_tiles", "rock_single_bottom", 55},

        {"swamp_autum_tiles", "rock_multi_top_left", 37},
        {"swamp_autum_tiles", "rock_multi_top", 38},
        {"swamp_autum_tiles", "rock_multi_top_right", 39},
        {"swamp_autum_tiles", "rock_multi_middle_left", 50},
        {"swamp_autum_tiles", "rock_multi_middle", 51},
        {"swamp_autum_tiles", "rock_multi_middle_right", 52},

        {"swamp_autum_tiles", "rock_multi_bottom_left", 62},
        {"swamp_autum_tiles", "rock_multi_bottom", 63},
        {"swamp_autum_tiles", "rock_multi_bottom_right", 64},
        {"swamp_autum_tiles", "wood_walk_piece", 86},

        {"swamp_autum_tiles", "grass_autum", 9},
        {"swamp_autum_tiles", "bush_autum_small", 8},
        {"swamp_autum_tiles", "bush_autum_large", 33},
        {"swamp_autum_tiles", "lilypad_autum_large", 32},

        {"swamp_autum_tiles", "fence_left", 56},
        {"swamp_autum_tiles", "fence_right", 57},
        {"swamp_autum_tiles", "fence", 58},
        
        {"swamp_autum_tiles", "rock_small", 77},
        {"swamp_autum_tiles", "rock_large", 76},

        // Weapons
        {"bat_sheet", "bat", 0},
        {"bat_sheet", "bat_swing", 1},
    };

    Assets::register_textures(textures);

    const std::vector<LoadAnimation> sprite_sheets {
        {"capybara_default_walk_anim_sheet", "capybara_default_walk_anim", 0, 8},
        {"capybara_default_idle_anim_sheet", "capybara_default_idle_anim", 0, 7},
        {"capybara_default_jump_anim_sheet", "capybara_default_jump_anim", 0, 7},
        {"capybara_default_duck_anim_sheet", "capybara_default_duck_anim", 0, 7},
        
        {"drone_idle_anim_sheet", "drone_idle_anim", 0, 7},
    };
    Assets::register_sprite_sheets(sprite_sheets);

    auto& window_controller = engine.services->get_service<RenderingService>().get().window();
    window_controller.set_window_fullscreen();
}

void Game::run() {
    const Engine & engine = Engine::instance();
    auto& scene_service = engine.services->get_service<SceneService>().get();

    // setup levels
    SwampScene::setup();
    SwampAutumScene::setup();

    auto load_create_game_scene = [](){};

    auto load_join_game_scene = [](const std::string& address){};

    auto load_training_scene = [&scene_service]() {
        scene_service.load_scene(SwampScene::SCENE_NAME);
    };

    MainMenuScene main_menu;
    Scene& main_menu_scene = main_menu.setup(
        load_create_game_scene,
        load_join_game_scene,
        load_training_scene
    );

    strap_pause_menu(main_menu_scene);
    scene_service.load_scene(main_menu_scene.name());
}

void Game::shutdown() {
    Engine& engine = Engine::instance();
    engine.quit();
}