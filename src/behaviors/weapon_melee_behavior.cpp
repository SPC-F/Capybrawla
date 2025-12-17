#include <game/behaviors/weapon_melee_behavior.h>

#include <engine/core/engine.h>
#include <engine/input/input_system.h>
#include <engine/input/input_manager.h>
#include <game/character/player_controller.h>

WeaponMeleeBehavior::WeaponMeleeBehavior(int damage, int range, int swingspeed, GameObject& hitbox_gameobject)
    : damage_(damage), range_(range), swingspeed_(swingspeed), hitbox_gameobject_(hitbox_gameobject) {}

void WeaponMeleeBehavior::on_awake() {
    auto maybe_sprite = this->get_component<Sprite>();
    if (!maybe_sprite) {
        throw std::runtime_error("WeaponMeleeBehavior requires a Sprite component.");
    }

    auto maybe_hitbox = hitbox_gameobject_.get().get_component<BoxCollider2D>();
    if (!maybe_hitbox) {
        throw std::runtime_error("WeaponMeleeBehavior requires a BoxCollider2D component.");
    }

    auto maybe_player = game_object().parent();
    if (!maybe_player) {
        throw std::runtime_error("WeaponMeleeBehavior requires a parent GameObject.");
    }

    sprite_component_ = maybe_sprite->get();
    hitbox_component_ = maybe_hitbox->get();
    player_component_ = maybe_player->get();

    hitbox_component_->get().width(static_cast<float>(range_));

    hitbox_component_->get().active(false);
    hitbox_component_->get().add_on_trigger_enter(
        [this](Collider2D& self, Collider2D& other) {
            auto other_parent_opt = other.parent();
            if (other_parent_opt.has_value()) {
                auto& other_gameobject = other_parent_opt->get();
                if (other_gameobject.tag() == "Player") return;
                
                auto behaviors = other_gameobject.get_components<BehaviorScript>();
                for (auto& behavior_ref : behaviors) {
                    auto& behavior = behavior_ref.get().behavior();

                    if (auto ctrl = dynamic_cast<PlayerControllerBehavior*>(&behavior); ctrl != nullptr) {
                        ctrl->damage(damage_);
                    }
                }
            }
        });
}

void WeaponMeleeBehavior::on_update(float dt) {
    const IInputProvider &provider =
        Engine::instance().services->get_service<InputManager>().get().provider();

    auto& rb = game_object().parent()->get().get_component<Rigidbody2D>()->get();

    if (rb.velocity().x > 0) {
        sprite_component_->get().flip_x(false);
        looking_right = true;
    }
    else if (rb.velocity().x < 0) {
        sprite_component_->get().flip_x(true);
        looking_right = false;
    }

    if (provider.is_mouse_pressed(MouseButton::left)) {
        if (sprite_component_) {
            sprite_component_->get().texture("bat_swing");

            hitbox_component_->get().active(true);

            auto player_c_width = player_component_->get().get_component<BoxCollider2D>()->get().width();

            float padding = 5.0f;

            float half_width = player_c_width * 0.5f;
            float forward_offset = 40.0f;
            float x_pos = looking_right ? forward_offset
                                        : -forward_offset;

            hitbox_gameobject_.get().transform().local_position({-range_ - padding, 0.0f, 0.0f});
            hitbox_timer_ = hitbox_duration_;
        }
    }

    if (hitbox_timer_ > 0.0f) {
        hitbox_timer_ -= dt;

        if (hitbox_timer_ <= 0.0f) {
            hitbox_component_->get().active(false);
            hitbox_gameobject_.get().transform().local_position({0.0f, 0.0f, 0.0f});
            hitbox_timer_ = 0.0f;
            
            if (sprite_component_) {
                sprite_component_->get().texture("bat");
            }
        }
    }
}
