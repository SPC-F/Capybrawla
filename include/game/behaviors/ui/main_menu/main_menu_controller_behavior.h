#pragma once

#include <engine/public/behavior.h>

class MainMenuControllerBehavior : public Behavior {
public:
    MainMenuControllerBehavior(int max_fall_width, int max_fall_height);

    void on_awake() override;
    void on_update(float dt);

private: 
    void create_capybaras(Scene& scene);
    void create_falling_capybara(Scene& scene, int index, Vector3 position = Vector3{});
    
    int max_fall_width_;
    int max_fall_height_;
    float accumulated_time_{0.0f}; 

    int original_falling_amount_{10};
    int falling_amount_{10};
    
    float original_time_between_falls_{2.0f};
    float time_between_falls_{2.0f};   
    
    std::vector<std::string> capybara_variants_{
        "capybara_default_idle",
        "capybara_red_idle",
        "capybara_blue_idle",
        "capybara_green_idle",
    };
};