#pragma once

#include "../assets.h"

#include <game/scenes/level.h>
#include <engine/public/scene.h>
#include <game/round/roundcontroller.h>

class SwampScene : public Level {
public:
    SwampScene();
    virtual ~SwampScene() = default;

    void setup(Scene& scene) override;
    void load(Scene& scene) override;
    
private:
    void load_players(Scene& scene, RoundController& controller, float start_x, float start_y);
    RoundController& add_round_controller(Scene& scene);
    void load_ai_agent(Scene& scene);
    void load_timer(Scene& scene);
    void load_interactables(Scene& scene);

    float out_of_bounds_margin_x_ = 480.0f;
    float out_of_bounds_margin_y_ = 270.0f;
    float map_width_ = 1920.0f;
    float map_height_ = 1080.0f;

    lib::Subscription on_player_lives_changed_subscription_;
    lib::Subscription on_ai_lives_changed_subscription_;
};