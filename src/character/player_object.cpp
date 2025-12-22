
#include <game/character/player_movement_behavior.h>
#include <game/character/player_object.h>
#include <game/character/player_controller.h>

#include "engine/public/components/animator.h"
#include "engine/public/components/behaviorscript.h"
#include "engine/public/components/colliders/box_collider_2d.h"
#include "engine/public/components/rigidbody_2d.h"
#include "engine/public/components/sprite.h"
#include "game/character/player_outofbounds_behavior.h"

PlayerObject::PlayerObject(Scene &scene, const Vector3 initial_pos, bool is_local_player)
    : GameObject(scene) {
  this->name("PlayerObject");
  this->tag("Player");
  this->transform().position(initial_pos);

  this->transform().scale({scale_factor_, scale_factor_, 1.0f});

  // how we look
  this->add_component<Sprite>("capybara_default_idle", Color(), 0, 0, 0, 0);
  this->add_component<Animator>("capybara_default_walk_anim", 95);

  // how we physics
  const Point default_offset = {default_x_offset_, 4 * scale_factor_};

  this->add_component<Rigidbody2D>(BodyType2D::Type::Dynamic, 35.0f);
  this->add_component<BoxCollider2D>(0.1f, 0.2f, 20 * scale_factor_,
                                     default_height_, default_offset);

  this->add_component<BehaviorScript>(std::make_unique<PlayerController>(100, 100));
}

void PlayerObject::set_controllable() {
  this->add_component<BehaviorScript>(std::make_unique<PlayerMovementBehavior>(
      default_height_, default_height_ / 2.0f, default_offset_,
      Point{default_x_offset_, default_height_ / 2.4 * scale_factor_}));
};
