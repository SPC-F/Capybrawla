#include <game/scenes/main_menu.h>

#include <../../include/game/game/game.h>
#include <game/assets.h>
#include <game/behaviors/ui_fall_behavior.h>

#include <memory>

#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/core/system/system_service.h>
#include <engine/public/camera.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/scene_service.h>

constexpr float BUTTON_WIDTH = 400.0f;
constexpr float BUTTON_HEIGHT = 80.0f;
constexpr float BUTTON_FONT_SIZE = 46.0f;

constexpr int SCREEN_WIDTH = 1920.0f;
constexpr int SCREEN_HEIGHT = 1080.0f;

constexpr int CENTER_X = SCREEN_WIDTH / 2;
constexpr int CENTER_Y = SCREEN_HEIGHT / 2;

constexpr int BUTTON_SPACING = 20;
constexpr float BUTTON_START_Y = CENTER_Y - ((BUTTON_HEIGHT + BUTTON_SPACING) * 2);
constexpr float BUTTON_START_X = CENTER_X - (BUTTON_WIDTH / 2);

constexpr float TITLE_Y = BUTTON_START_Y - 200.0f;
constexpr float TITLE_X = CENTER_X - 250.0f;
constexpr float TITLE_FONT_SIZE = 180.0f;

Scene& MainMenuScene::setup(
    create_callback_t create_callback,
    join_callback_t join_callback,
    training_callback_t training_callback
) {
    const std::string main_menu_scene_tag = "MainMenuScene";

    SceneService& scene_service = Engine::instance().services->get_service<SceneService>().get();
    Scene& scene = scene_service.add_scene(main_menu_scene_tag);
    scene.add_game_object<Camera>(scene, Color(), 1.0f, true);

    GameObject& bg = scene.add_game_object("Background");
    bg.add_component<Sprite>("main_menu_bg", Color{255, 255, 255, 255}, 0, 0, 0, 0);
    bg.transform().position({0, 0, 0});

    auto add_menu_parent = [&scene, this](const std::string& parent_name, bool active) -> GameObject& {
        GameObject& parent = scene.add_game_object(parent_name);
        parents_.emplace(parent_name, parent);
        
        if (!active) {
            parent.set_inactive_in_world();
            parent.set_inactive();
        }

        return parent;
    };
    
    GameObject& main_menu_parent = add_menu_parent("MainMenuParent", true);
    setup_main_menu(scene, main_menu_parent, training_callback);

    GameObject& create_game_parent = add_menu_parent("CreateGameParent", false);
    setup_create_game(scene, create_game_parent, create_callback);

    GameObject& join_game_parent = add_menu_parent("JoinGameParent", false);
    setup_join_game(scene, join_game_parent, join_callback);

    GameObject& settings_parent = add_menu_parent("SettingsParent", false);
    setup_settings(scene, settings_parent);

    GameObject& credits_parent = add_menu_parent("CreditsParent", false);
    setup_credits(scene, credits_parent);

    falling_capybaras(scene, 10);

    return scene;
}

void MainMenuScene::setup_main_menu(Scene& scene, GameObject& parent, training_callback_t training_callback) {
    SceneService& scene_service = Engine::instance().services->get_service<SceneService>().get();

    UIText& title_text = create_text(
        scene,
        "Capybrawla",
        TITLE_X,
        TITLE_Y,
        600.0f,
        200.0f,
        TITLE_FONT_SIZE,
        Color{255, 255, 255, 255}
    );
    title_text.parent(parent);

    UIButton& create_game_button = create_button(
        scene,
        "Create game",
        BUTTON_START_X,
        BUTTON_START_Y,
        "button_large_blue"
    );
    create_game_button.parent(parent);
    create_game_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("CreateGameParent");
    });

    UIButton& training_button = create_button(
        scene,
        "Training",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 1,
        "button_large_green"
    );
    training_button.parent(parent);
    training_button.add_on_press([this, training_callback](UIButton& btn) {
        auto& system_service = Engine::instance().services->get_service<SystemService>().get();
        system_service.set_cursor_to_arrow();

        training_callback();
    });

    UIButton& join_game_button = create_button(
        scene,
        "Join game",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 2,
        "button_large_green"
    );
    join_game_button.parent(parent);
    join_game_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("JoinGameParent");
    });

    UIButton& settings_button = create_button(
        scene,
        "Settings",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 3,
        "button_large_yellow"
    );
    settings_button.parent(parent);
    settings_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("SettingsParent");
    });

    UIButton& credits_button = create_button(
        scene,
        "Credits",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 4,
        "button_large_yellow"
    );
    credits_button.parent(parent);
    credits_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("CreditsParent");
    });

    UIButton& exit_button = create_button(
        scene,
        "Quit",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 5,
        "button_large_red"
    );
    exit_button.parent(parent);
    exit_button.add_on_press([this, &scene_service](UIButton& /*btn*/) {
        if (auto current_scene_opt = scene_service.current_scene(); current_scene_opt.has_value()) {
            current_scene_opt->get().stop();
        }
    });
}

void MainMenuScene::setup_create_game(Scene& scene, GameObject& parent, create_callback_t create_callback) {
    UIButton& back_button = create_button(
        scene,
        "Back",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 4,
        "button_large_red"
    );
    back_button.parent(parent);
    back_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("MainMenuParent");
    });
}

void MainMenuScene::setup_join_game(Scene& scene, GameObject& parent, join_callback_t join_callback) {
    UIInput& address_input = create_input(
        scene,
        CENTER_X - (BUTTON_WIDTH / 2),
        CENTER_Y,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Enter address..."
    );
    address_input.parent(parent);
    
    UIButton& join_button = create_button(
        scene,
        "Join",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 3,
        "button_large_green"
    );
    join_button.parent(parent);
    join_button.add_on_press([this, join_callback, &address_input](UIButton& /*btn*/) {
        auto& system_service = Engine::instance().services->get_service<SystemService>().get();
        system_service.set_cursor_to_arrow();
        
        join_callback(address_input.text());
    });
    
    UIButton& back_button = create_button(
        scene,
        "Back",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 4,
        "button_large_red"
    );
    back_button.parent(parent);
    back_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("MainMenuParent");
    });
}

void MainMenuScene::setup_settings(Scene& scene, GameObject& parent) {
    auto& rendering_service = Engine::instance().services->get_service<RenderingService>().get();
    bool vsync = rendering_service.vsync();

    UIButton& vsync_button = create_button(
        scene,
        vsync ? "Toggle VSync: OFF" : "Toggle VSync: ON",
        BUTTON_START_X,
        BUTTON_START_Y,
        "button_large_blue"
    );
    vsync_button.parent(parent);
    vsync_button.add_on_press([this, &rendering_service](UIButton& btn) {
        bool current_vsync = rendering_service.vsync();
        rendering_service.vsync(!current_vsync);

        btn.label(current_vsync ? "Toggle VSync: ON" : "Toggle VSync: OFF");
    });

    UIButton& back_button = create_button(
        scene,
        "Back",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 4,
        "button_large_red"
    );
    back_button.parent(parent);
    back_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("MainMenuParent");
    });
}

void MainMenuScene::setup_credits(Scene& scene, GameObject& parent) {
    auto create_credits_text_shortcut = [this, &scene, &parent](const std::string& label, float offset_y) {
        UIText& text = create_text(
            scene,
            label,
            CENTER_X - 200.0f,
            CENTER_Y - 100.0f + offset_y,
            400.0f,
            50.0f,
            36,
            Color{255, 255, 255, 255}
        );
        text.parent(parent);
    };

    create_credits_text_shortcut("This game was created for the minor System Programming in C++ at Avans University of Applied Sciences.", -140.0f);
    create_credits_text_shortcut("Our vision was to create a fun multiplayer brawling game", -100.0f);
    create_credits_text_shortcut("featuring adorable capybaras as the main characters.", -60.0f);

    create_credits_text_shortcut("Developed by", 0.0f);
    create_credits_text_shortcut("- Lars", 40.0f);
    create_credits_text_shortcut("- Luka", 80.0f);
    create_credits_text_shortcut("- Thomas", 120.0f);
    create_credits_text_shortcut("- Sjul", 160.0f);
    create_credits_text_shortcut("Using Capycore Engine", 240.0f);

    UIButton& back_button = create_button(
        scene,
        "Back",
        BUTTON_START_X,
        BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 4,
        "button_large_red"
    );
    back_button.parent(parent);
    back_button.add_on_press([this](UIButton& /*btn*/) {
        toggle_parent_visibility("MainMenuParent");
    });
}

void MainMenuScene::falling_capybaras(Scene& scene, int count) {
    std::string capybaras[] = {
        "capybara_default_idle",
        "capybara_red_idle",
        "capybara_blue_idle",
        "capybara_green_idle",
    };

    for (int i = 0; i < count; ++i) {
        float size_modifier     = 2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 3.8f));
        float rotation_speed    = 20.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 80.0f));
        float fall_speed        = 50.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 150.0f));
        float position_x        = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / SCREEN_WIDTH));
        float position_y        = -100.0f - static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 500.0f));
        float capybara_choice   = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 5.0f));

        std::string chosen_capybara = capybaras[static_cast<int>(capybara_choice) % (sizeof(capybaras) / sizeof(capybaras[0]))];

        GameObject& falling_object = scene.add_game_object("Falling_Capybara_" + std::to_string(i));
        falling_object.transform().position({position_x, position_y, 0.0f});
        falling_object.transform().scale({size_modifier, size_modifier, 1.0f});
        falling_object.add_component<Sprite>(chosen_capybara, Color{255, 255, 255, 255}, 0, 0, 0, 0);
        falling_object.add_component<BehaviorScript>(std::make_unique<UIFallBehavior>(rotation_speed, fall_speed, static_cast<float>(SCREEN_HEIGHT)));
        falling_object.layer(Layers::Foreground);
    }

}

void MainMenuScene::toggle_parent_visibility(const std::string& parent_name) {
    for (auto& [name, parent] : parents_) {
        if (name == parent_name) {
            parent.set_active_in_world();
            parent.set_active();
        } 
        else {
            parent.set_inactive_in_world();
            parent.set_inactive();
        }
    }
}

UIInput& MainMenuScene::create_input(
    Scene& scene,
    float position_x,
    float position_y,
    float width,
    float height,
    const std::string& placeholder_text
) {
    UIInput& input = scene.add_game_object<UIInput>(
        scene,
        width,
        height,
        Point{0.5f, 0.5f},
        Point{0.5f, 0.5f},
        placeholder_text,
        "ByteBounce",
        "resources/fonts/bytebounce/ByteBounce.ttf",
        "button_big_hollow"
    );
    input.transform().position({position_x, position_y, 0.0f});
    input.font_size(BUTTON_FONT_SIZE);
    input.layer(Layers::UI);

    return input;
}

UIText& MainMenuScene::create_text(
    Scene& scene,
    const std::string& label, 
    float position_x,
    float position_y,
    float width,
    float height,
    int font_size,
    const Color& color
) {
    UIText& text = scene.add_game_object<UIText>(
        scene,
        label,
        "ByteBounce",
        "resources/fonts/bytebounce/ByteBounce.ttf",
        width,
        height,
        Point{0.5f, 0.5f},
        Point{0.5f, 0.5f}
    );
    text.transform().position({position_x, position_y, 0.0f});
    text.font_size(font_size);
    text.color(color);
    text.layer(Layers::UI);

    return text;
}

UIButton& MainMenuScene::create_button(
    Scene& scene,
    const std::string& label, 
    float position_x,
    float position_y, 
    const std::string& texture_id
) {
    UIButton& button = scene.add_game_object<UIButton>(
        scene,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        Point{0.5f, 0.5f},
        Point{0.5f, 0.5f},
        label,
        "ByteBounce",
        "resources/fonts/bytebounce/ByteBounce.ttf",
        texture_id
    );
    button.transform().position({position_x, position_y, 0.0f});
    button.label_color(Color{255, 255, 255, 255});
    button.font_size(BUTTON_FONT_SIZE);
    button.layer(Layers::UI);

    return button;
}

