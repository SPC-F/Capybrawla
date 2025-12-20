#include <game/behaviors/ui/main_menu/main_menu_controller_behavior.h>

#include <engine/core/engine.h>
#include <engine/public/scene.h>
#include <engine/input/input_manager.h>
#include <engine/input/input_system.h>
#include <game/prefabs/character/falling_capybara_object.h>

MainMenuControllerBehavior::MainMenuControllerBehavior(int max_fall_width, int max_fall_height)
    : max_fall_width_(max_fall_width), max_fall_height_(max_fall_height) {}

void MainMenuControllerBehavior::on_awake() {
    create_capybaras(this->game_object().scene());
}

void MainMenuControllerBehavior::on_update(float dt) {
    auto& provider = Engine::instance().services->get_service<InputManager>().get().provider();

    if (provider.is_key_pressed(KeyCode::arrow_up))         falling_amount_ += 10;
    else if (provider.is_key_pressed(KeyCode::arrow_down))  falling_amount_ = std::max(0, falling_amount_ - 1);

    if (provider.is_key_pressed(KeyCode::arrow_right))      time_between_falls_ += 0.5f;
    else if (provider.is_key_pressed(KeyCode::arrow_left))  time_between_falls_ = std::max(0.1f, time_between_falls_ - 0.5f);  

    if (provider.is_mouse_pressed(MouseButton::left)) {
        Point mouse_pos = provider.mouse_position();
        create_falling_capybara(this->game_object().scene(), 0, Vector3{static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y), 0.0f});
    }
    
    if (provider.is_key_pressed(KeyCode::right_shift)) {
        falling_amount_       = original_falling_amount_;
        time_between_falls_   = original_time_between_falls_;
    }

    /// Do not spawn more capybaras until a certain amount of time has elapsed
    accumulated_time_ += dt;    
    if (accumulated_time_ < time_between_falls_) {
        return;
    }
    accumulated_time_ = 0.0f;

    create_capybaras(this->game_object().scene());
}

void MainMenuControllerBehavior::create_capybaras(Scene& scene) {
    for (int i = 0; i < falling_amount_; ++i) {
        create_falling_capybara(scene, i);
    }
}

void MainMenuControllerBehavior::create_falling_capybara(Scene& scene, int index, Vector3 position) {
    float size_modifier     = 2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 3.8f));
    float rotation_speed    = 20.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 80.0f));
    float fall_speed        = 50.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 150.0f));
    float position_x        = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / max_fall_width_));
    float position_y        = -100.0f - static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / max_fall_height_));
    float capybara_choice   = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 5.0f));


    if (position.x == 0.0f && position.y == 0.0f) {
        position.x = position_x;
        position.y = position_y;
    }

    scene.add_game_object<FallingCapybaraObject>(
        scene, 
        max_fall_height_, 
        size_modifier, 
        rotation_speed, 
        fall_speed, 
        position.x, 
        position.y
    );
}