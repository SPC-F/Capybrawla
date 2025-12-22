#pragma once

#include <engine/public/gameObject.h>
#include <engine/public/scene.h>
#include <game/character/player_controller.h>
#include <lib/subscription.h>

namespace PlayerInfoComponent {
    class HealthBar : public GameObject {
    private:
        lib::Subscription _health_changed_subscription;

    public:
        HealthBar(Scene &scene,
                  PlayerController &controller,
                  float health_bar_width,
                  float health_bar_height,
                  float health_bar_offset_left,
                  float health_bar_offset_top,
                  float death_banner_width,
                  float death_banner_height);
    };
}
