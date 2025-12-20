#pragma once

#include <engine/public/gameObject.h>
#include <game/character/player_controller.h>
#include <lib/subscription.h>
#include "engine/public/scene.h"
#include "engine/public/ui/ui_image.h"
#include "engine/public/ui/ui_object.h"
#include <game/character/gui/player_info_component.h>

namespace PlayerInfoComponent {
    class LivesBar : public GameObject {
    private:
        lib::Subscription _lives_changed_subscription;
        std::vector<std::reference_wrapper<UIImage>> life_images_;

    public:
        LivesBar(Scene &scene,
                 PlayerController &controller,
                 float lives_bar_offset_left,
                 float lives_bar_offset_top,
                 float life_width,
                 float life_height,
                 float lives_bar_internal_offset);
    };
}
