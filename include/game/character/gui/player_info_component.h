#pragma once

#include <engine/public/gameObject.h>
#include <game/character/player_object.h>
#include <game/character/player_controller.h>
#include <lib/subscription.h>

namespace PlayerInfoComponent {
    GameObject &create_and_add(Scene &scene, PlayerObject &player);

    class HealthBar : public GameObject {
    private:
        lib::Subscription _health_changed_subscription;
    public:
        HealthBar(Scene &scene, PlayerControllerBehavior &controller);
    };

    class LivesBar : public GameObject {
    private:
        lib::Subscription _lives_changed_subscription;
    public:
        LivesBar(Scene &scene,
                 PlayerControllerBehavior &controller);
    };
};
