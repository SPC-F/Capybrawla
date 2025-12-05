#pragma once

#include <game/assets.h>

#include <map>
#include <functional>

#include <engine/public/scene.h>
#include <engine/public/gameObject.h>
#include <engine/public/ui/interactable/ui_button.h>
#include <engine/public/ui/interactable/ui_input.h>
#include <engine/public/ui/ui_text.h>

using join_callback_t = std::function<void(const std::string& address)>;
using create_callback_t = std::function<void()>;
using training_callback_t = std::function<void()>;

class MainMenuScene {
public:
    Scene& setup(
        create_callback_t create_callback,
        join_callback_t join_callback,
        training_callback_t training_callback
    );

    void setup_main_menu(
        Scene& scene,
        GameObject& parent,
        training_callback_t training_callback
    );

    void setup_create_game(
        Scene& scene, 
        GameObject& parent,
        create_callback_t create_callback
    );

    void setup_join_game(
        Scene& scene,
        GameObject& parent,
        join_callback_t join_callback
    );

    void setup_settings(Scene& scene, GameObject& parent);
    void setup_credits(Scene& scene, GameObject& parent);
    void falling_capybaras(Scene& scene, int count);

private:
    std::map<std::string, GameObject&> parents_;

    void toggle_parent_visibility(const std::string& parent_name);

    UIInput& create_input(
        Scene& scene,
        float position_x,
        float position_y,
        float width,
        float height,
        const std::string& placeholder_text
    );

    UIText& create_text(
        Scene& scene,
        const std::string& label, 
        float position_x,
        float position_y,
        float width,
        float height,
        int font_size,
        const Color& color
    );
    
    UIButton& create_button(
        Scene& scene,
        const std::string& label, 
        float position_x,
        float position_y,
        const std::string& texture_id
    );
};