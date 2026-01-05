#include <game/behaviors/interactable/weapon_sword.h>

#include <game/prefabs/weapons/weapon_sword_player_object.h>
#include <game/prefabs/config/weapon_sword_config.h>
#include <game/character/player_weapon_controller.h>

#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/colliders/collider_2d.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/gameObject.h>

void WeaponSwordInteractableBehavior::on_start() {
    auto collider_opt = get_component<BoxCollider2D>();
    if (!collider_opt.has_value()) {
        throw std::runtime_error("WeaponSwordInteractableBehavior requires a BoxCollider2D component.");
    }

    collider_opt->get().add_on_trigger_enter([this](Collider2D& self, Collider2D& other) {
        if (time_since_spawn_ < pickup_delay_) return;

        auto behavior_opt = self.parent()->get().get_component<BehaviorScript>();
        if (!behavior_opt.has_value()) return;

        auto& behavior = behavior_opt->get().behavior();
        auto* sword_behavior = dynamic_cast<WeaponSwordInteractableBehavior*>(&behavior);
        if (!sword_behavior) return;

        sword_behavior->pickup_weapon(self, other);
    });

    collider_opt->get().add_on_collision_enter([this](Collider2D& self, Collider2D& other) {
        if (time_since_spawn_ < pickup_delay_) return;

        auto behavior_opt = self.parent()->get().get_component<BehaviorScript>();
        if (!behavior_opt.has_value()) return;

        auto& behavior = behavior_opt->get().behavior();
        auto* sword_behavior = dynamic_cast<WeaponSwordInteractableBehavior*>(&behavior);
        if (!sword_behavior) return;

        sword_behavior->pickup_weapon(self, other);
    });
}

void WeaponSwordInteractableBehavior::pickup_weapon(Collider2D& self, Collider2D& other) {
    auto current_opt = other.parent();
    while (current_opt.has_value()) {
        auto& obj = current_opt->get();
        if (obj.tag() == "Player") {
            auto controller_opt = obj.get_script<BehaviorScript, PlayerWeaponController>();
            if (!controller_opt.has_value() || controller_opt->get().has_found_weapon()) return;

            controller_opt->get().switch_weapon<WeaponSwordPlayerObject>();
            self.parent()->get().mark_for_deletion();
            break;
        }
        current_opt = obj.parent();
    }
}

void WeaponSwordInteractableBehavior::on_update(float dt) {
    auto rb_opt = this->game_object().get_component<Rigidbody2D>();

    if (rb_opt.has_value()) {
        auto& rb = rb_opt->get();
        rb.velocity({0.0f, rb.velocity().y, 0.0f});
    }

    if (time_since_spawn_ > destroy_delay_) {
        this->game_object().mark_for_deletion();
        return;
    }
    
    time_since_spawn_ += dt;
}