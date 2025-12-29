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
}

void CloudPlatformBehavior::on_update(float dt) {
    if (!player_left_) return;

    accumulated_time_ += dt;

    if (accumulated_time_ >= time_before_disappear_) {
        game_object().mark_for_deletion();
    }
}