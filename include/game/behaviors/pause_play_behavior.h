#pragma once

#include <engine/core/engine.h>
#include <engine/input/input_manager.h>
#include <engine/public/behavior.h>

class PausePlayBehavior final : public Behavior
{
public:
    explicit PausePlayBehavior(const std::reference_wrapper<PauseMenuUI> pause_menu)
        : input_(Engine::instance().services->get_service<InputManager>().get().provider()),
          scene_service_(Engine::instance().services->get_service<SceneService>().get()),
          pause_menu_(pause_menu.get())
    {}

    void on_awake() override
    {
        pause_menu_.set_inactive();
        pause_menu_.set_resume_press_callback([this]()
        {
            is_paused_ = false;
            update_ui_state();
        });
        pause_menu_.set_quit_press_callback([this]()
        {
            is_paused_ = false;
            update_ui_state();
        });
    }

    void on_update(float dt) override
    {
         if (input_.is_key_pressed(KeyCode::escape))
         {
            if (!scene_service_.current_scene().value().get().name().starts_with("Level_")) return;

             is_paused_ = !is_paused_;
             update_ui_state();
         }
    }

private:
    const IInputProvider& input_;
    const SceneService& scene_service_;
    bool is_paused_ = false;
    PauseMenuUI& pause_menu_;

    void update_ui_state() const
    {
        if (is_paused_) pause_menu_.set_active();
        else pause_menu_.set_inactive();
    }
};
