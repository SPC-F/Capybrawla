#pragma once
#include <string_view>

enum class PlayerColor : uint16_t {
    DEFAULT = 0,
    RED,
    BLUE,
    GREEN
};

struct PlayerAnimationConstants {
    std::string_view walking_animation;
    std::string_view idle_animation;
    std::string_view jumping_animation;
    std::string_view crouching_animation;
    std::string_view death_animation;
    std::string_view hit_animation;
    
    std::string_view idle_texture;
    std::string_view crouching_texture;
    std::string_view jumping_texture;
    std::string_view death_texture;
    std::string_view hit_texture;
    std::string_view head_texture;
};

namespace PlayerConstants {
    constexpr PlayerAnimationConstants get_constants(PlayerColor color) {
        switch (color) {
            case PlayerColor::RED:
                return {
                    "capybara_red_walk_anim",
                    "capybara_red_idle_anim",
                    "capybara_red_jump_anim",
                    "capybara_red_duck_anim",
                    "capybara_red_death_anim",
                    "capybara_red_hit_anim",
                    "capybara_red_idle",
                    "capybara_red_duck",
                    "capybara_red_jump",
                    "capybara_red_death",
                    "capybara_red_hit",
                    "capybara_red_head"
                };
            case PlayerColor::BLUE:
                return {
                    "capybara_blue_walk_anim",
                    "capybara_blue_idle_anim",
                    "capybara_blue_jump_anim",
                    "capybara_blue_duck_anim",
                    "capybara_blue_death_anim",
                    "capybara_blue_hit_anim",
                    "capybara_blue_idle",
                    "capybara_blue_duck",
                    "capybara_blue_jump",
                    "capybara_blue_death",
                    "capybara_blue_hit",
                    "capybara_blue_head"
                };
            case PlayerColor::GREEN:
                return {
                    "capybara_green_walk_anim",
                    "capybara_green_idle_anim",
                    "capybara_green_jump_anim",
                    "capybara_green_duck_anim",
                    "capybara_green_death_anim",
                    "capybara_green_hit_anim",
                    "capybara_green_idle",
                    "capybara_green_duck",
                    "capybara_green_jump",
                    "capybara_green_death",
                    "capybara_green_hit",
                    "capybara_green_head"
                };
            default: // PlayerColor::DEFAULT
                return {
                    "capybara_default_walk_anim",
                    "capybara_default_idle_anim",
                    "capybara_default_jump_anim",
                    "capybara_default_duck_anim",
                    "capybara_default_death_anim",
                    "capybara_default_hit_anim",
                    "capybara_default_idle",
                    "capybara_default_duck",
                    "capybara_default_jump",
                    "capybara_default_death",
                    "capybara_default_hit",
                    "capybara_default_head"
                };
        }
    }
}