#include <game/behaviors/weapon_melee_behavior.h>

#include <game/character/player_controller.h>
#include <engine/core/engine.h>
#include <engine/input/input_system.h>
#include <engine/input/input_manager.h>
#include <engine/network/multiplayer_service.h>
#include <engine/public/components/animator.h>
#include <engine/public/components/network_identity.h>

WeaponMeleeBehavior::WeaponMeleeBehavior(
    const std::string& attack_animation_name,
    const std::string& original_texture_name,
    int damage, 
    int range, 
    Point knockback_force, 
    GameObject& hitbox_gameobject,
    GameObject& sprite_gameobject,
    Point hitbox_offset,
    Point sprite_offset_left,
    Point sprite_offset_right,
    Point animator_offset_left,
    Point animator_offset_right
)
: attack_animation_name_(attack_animation_name),
    original_texture_name_(original_texture_name), 
    damage_(damage), 
    range_(range), 
    knockback_force_(knockback_force), 
    hitbox_gameobject_(hitbox_gameobject),
    sprite_gameobject_(sprite_gameobject),
    hitbox_offset_(hitbox_offset),
    sprite_offset_left_(sprite_offset_left),
    sprite_offset_right_(sprite_offset_right),
    animator_offset_left_(animator_offset_left),
    animator_offset_right_(animator_offset_right)
{}
    
void WeaponMeleeBehavior::on_awake() {
    auto maybe_sprite = sprite_gameobject_.get().get_component<Sprite>();
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

    hitbox_component_ = maybe_hitbox->get();
    player_component_ = maybe_player->get();
    sprite_component_ = maybe_sprite->get();

    hitbox_component_->get().width(static_cast<float>(range_));

    hitbox_component_->get().active(false);
    hitbox_component_->get().add_on_trigger_enter(
        [this](Collider2D& self, Collider2D& other) {
            auto other_parent_opt = other.parent();
            if (other_parent_opt.has_value()) {
                auto& other_gameobject = other_parent_opt->get();
                if (other_gameobject.tag() != "Player" || other_gameobject.id() == player_component_->get().id()) return;

                auto behaviors = other_gameobject.get_components<BehaviorScript>();
                for (auto& behavior_ref : behaviors) {
                    auto& behavior = behavior_ref.get().behavior();

                    if (auto ctrl = dynamic_cast<PlayerControllerBehavior*>(&behavior); ctrl != nullptr) {
                        ctrl->damage(damage_);
                    }
                }

                auto& rb = other_gameobject.get_component<Rigidbody2D>()->get();
                Point knockback = facing_right_ ? knockback_force_ : Point{-knockback_force_.x, -knockback_force_.y};
                rb.apply_impulse(Vector3{knockback.x, knockback.y, 0.0f});
            }
        });
}

void WeaponMeleeBehavior::on_update(float dt) {
    const IInputProvider &provider =
        Engine::instance().services->get_service<InputManager>().get().provider();

    auto& rb = game_object().parent()->get().get_component<Rigidbody2D>()->get();
    auto& animator = sprite_gameobject_.get().get_component<Animator>()->get();

    bool flipped = sprite_component_->get().flip_x();

    if (rb.velocity().x > 0)        facing_right_ = true;
    else if (rb.velocity().x < 0)   facing_right_ = false;

    sprite_component_->get().flip_x(!facing_right_);
    bool new_flipped = flipped != sprite_component_->get().flip_x();

    /// Set sprite position based on direction
    /// Only if not attacking due to animation offset
    if (!animator.is_playing() || new_flipped) {
        Point offset = facing_right_ ? sprite_offset_right_ : sprite_offset_left_;
        sprite_gameobject_.get()
            .transform()
            .local_position({ offset.x, offset.y, 0.0f });
    }

    /// Reset after attack animation is done, or if direction changed mid-attack
    if (!animator.is_playing() || new_flipped) {
        animator.reset();
        sprite_component_->get().texture(original_texture_name_);
        
        hitbox_component_->get().active(false);
        hitbox_gameobject_.get().transform().local_position({0.0f, -100.0f, 0.0f});
    }

    if (!is_multiplayer_and_local()) return;
    
    /// Set all the right positions and activate hitbox
    if (provider.is_mouse_pressed(MouseButton::left) && !animator.is_playing()) {
        attack();
    }
}

bool WeaponMeleeBehavior::is_multiplayer_and_local() {
    auto network_identity = game_object().parent()->get().get_component<NetworkIdentity>();
    if (network_identity.has_value() && !network_identity->get().uuid().empty()) {
        auto uuid = network_identity->get().uuid();
        auto multiplayer_uuid = Engine::instance().services->get_service<MultiplayerService>().get().get_uuid();

        return multiplayer_uuid == uuid;
    }

    return true;
}

void WeaponMeleeBehavior::attack() {
    auto& animator = sprite_gameobject_.get().get_component<Animator>()->get();

    if (sprite_component_) {
        animator.set_animation(attack_animation_name_);

        Point animator_offset = facing_right_ ? animator_offset_right_ : animator_offset_left_;
        sprite_gameobject_.get()
            .transform()
            .local_position({ animator_offset.x, animator_offset.y, 0.0f });

        animator.play(false);
        hitbox_component_->get().active(true);

        float pos_x = hitbox_offset_.x;
        if (!facing_right_) {
            pos_x = -static_cast<float>(range_);
        }

        hitbox_gameobject_.get().transform().local_position({ pos_x, 0.0f, 0.0f });
    }
}