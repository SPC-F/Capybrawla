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

PlayerObject::PlayerObject(Scene& scene, const Vector3 initial_pos) : GameObject(scene) {
  this->name("PlayerObject");
  this->transform().position(initial_pos);

  constexpr float scale_factor = 1.5f;

  this->transform().scale({scale_factor, scale_factor, 1.0f});

  // how we look
  this->add_component<Sprite>("capybara_default", Color(), 0, 0, 0, 0);
  this->add_component<Animator>("WALKcapybara_anim", 95);

  // how we physics
  this->add_component<Rigidbody2D>(BodyType2D::Type::Dynamic, 35.0f);
  this->add_component<BoxCollider2D>(0.1f, 0.2f, 18 * scale_factor, 28 * scale_factor, Point{6 * scale_factor, 0});

  // how we behave
  this->add_component<BehaviorScript>(std::make_unique<PlayerMovementBehavior>());
}