//
// Created by thoma on 05/12/2025.
//

#include <game/character/player_movement_behavior.h>
#include <game/character/player_object.h>

#include "engine/public/components/animator.h"
#include "engine/public/components/behaviorscript.h"
#include "engine/public/components/colliders/box_collider_2d.h"
#include "engine/public/components/rigidbody_2d.h"
#include "engine/public/components/sprite.h"

PlayerObject::PlayerObject(Scene &scene, const Vector3 initial_pos)
    : GameObject(scene) {
  this->name("PlayerObject");
  this->transform().position(initial_pos);

  constexpr float scale_factor = 2.0f;

  this->transform().scale({scale_factor, scale_factor, 1.0f});

  // how we look
  this->add_component<Sprite>("capybara_default_idle", Color(), 0, 0, 0, 0);
  this->add_component<Animator>("capybara_default_walk_anim", 95);

  // how we physics
  constexpr float default_height = 28 * scale_factor;
  constexpr float default_x_offset = 6 * scale_factor;
  const Point default_offset = {default_x_offset, 4 * scale_factor};

  this->add_component<Rigidbody2D>(BodyType2D::Type::Dynamic, 35.0f);
  this->add_component<BoxCollider2D>(0.1f, 0.2f, 20 * scale_factor,
                                     default_height, default_offset);

  // how we behave
  this->add_component<BehaviorScript>(std::make_unique<PlayerMovementBehavior>(
      default_height, default_height / 2.0f, default_offset,
      Point{default_x_offset, default_height / 2.4 * scale_factor}));
}