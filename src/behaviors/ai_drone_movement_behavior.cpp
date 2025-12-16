#include <game/behaviors/ai_drone_movement_behavior.h>

void AIDroneMovementBehavior::on_awake() {
    auto maybe_sprite = this->get_component<Sprite>();
    if (!maybe_sprite) {
        throw std::runtime_error("AIDroneMovementBehavior requires a Sprite component.");
    }

    auto maybe_rigidbody = this->get_component<Rigidbody2D>();
    if (!maybe_rigidbody) {
        throw std::runtime_error("AIDroneMovementBehavior requires a Rigidbody2D component.");
    }

    sprite_component_ = maybe_sprite->get();
    rigidbody_component_ = maybe_rigidbody->get();
}

void AIDroneMovementBehavior::on_update(float dt) {
    if (!rigidbody_component_ || !sprite_component_) return;
    
    auto& transform = this->transform();
    auto& rigidbody = rigidbody_component_->get();
    auto& sprite = sprite_component_->get();
    
    Vector3 velocity = rigidbody.velocity();
    bool should_flip_x = velocity.x < 0;

    sprite.flip_x(should_flip_x);
}