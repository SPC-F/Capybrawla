#include <game/behaviors/cloud_platform_behavior.h>

#include <engine/public/components/colliders/collider_2d.h>

void CloudPlatformBehavior::on_start() {
    auto maybe_box_collider = this->get_component<BoxCollider2D>();
    if (!maybe_box_collider) {
        throw std::runtime_error("CloudPlatformBehavior requires a BoxCollider2D component.");
    }

    box_collider_ = maybe_box_collider->get();

    box_collider_->get().add_on_collision_exit([this](Collider2D& self, Collider2D& other) {
        auto other_parent_opt = other.parent();
        if (other_parent_opt.has_value()) {
            auto& other_gameobject = other_parent_opt->get();
            if (other_gameobject.tag() != "Player") return;

            if (!player_left_) {
                player_left_ = true;
                accumulated_time_ = 0.0f;
            }
        }
    });

    disappearing_ = false;
    disappear_timer_ = 0.0f;
}

void CloudPlatformBehavior::on_update(float dt) {
    if (disappearing_) {
        auto pos = game_object().transform().position();
        pos.y -= disappear_speed_ * dt;
        game_object().transform().position(pos);
  
        disappear_timer_ += dt;
        if (disappear_timer_ >= disappear_duration_) game_object().mark_for_deletion();
        return;
    }

    if (!player_left_) return;

    accumulated_time_ += dt;

    if (accumulated_time_ >= time_before_disappear_) {
        disappearing_ = true;
        disappear_timer_ = 0.0f;
    }
}