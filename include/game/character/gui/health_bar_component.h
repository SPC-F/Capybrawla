#pragma once

#include <engine/public/gameObject.h>
#include <game/character/player_controller.h>
#include <lib/subscription.h>

#include "engine/public/scene.h"
#include "engine/public/ui/ui_image.h"
#include "engine/public/ui/ui_object.h"
#include "engine/public/ui/ui_text.h"

#include <game/character/gui/player_info_component.h>

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
