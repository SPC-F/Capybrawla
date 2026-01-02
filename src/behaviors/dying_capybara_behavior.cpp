#include <game/behaviors/dying_capybara_behavior.h>

#include <engine/public/components/animator.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>

DyingCapybaraBehavior::DyingCapybaraBehavior(float disappear_time)
    : time_before_disappear_(disappear_time) {}

void DyingCapybaraBehavior::on_awake() {
    auto maybe_animator = get_component<Animator>();
    if (maybe_animator) {
        auto& animator = maybe_animator->get();
        animator.play(false);
    }
}

void DyingCapybaraBehavior::on_update(float dt) {
    accumulated_time_ += dt;

    auto maybe_rigidbody = get_component<Rigidbody2D>();
    if (maybe_rigidbody) {
        auto& rb = maybe_rigidbody->get();
        rb.velocity({0.0f, rb.velocity().y, 0.0f});
    }

    if (accumulated_time_ >= time_before_disappear_) game_object().mark_for_deletion();
}