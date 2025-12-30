#include <game/prefabs/character/dying_capybara_object.h>
#include <game/behaviors/ui/main_menu/main_menu_fall_behavior.h>
#include <game/behaviors/dying_capybara_behavior.h>
#include <game/character/playerConstants.h>

#include <engine/public/scene.h>
#include <engine/public/components/animator.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>

constexpr float size_modifier = 2.0f;

DyingCapybaraObject::DyingCapybaraObject(Scene& scene, Vector3 position, float disappear_time)
: GameObject(scene),
  disappear_time_(disappear_time)
  {
    this->name("Dying_Capybara_Object");
    this->transform().scale({size_modifier, size_modifier, 1.0f});
    this->transform().position(position);
    this->layer(Layers::Foreground);
    
    this->add_component<Sprite>(PlayerConstants::IDLE_TEXTURE, Color{255, 255, 255, 255}, 0, 0, 0, 0);
    this->add_component<Animator>(PlayerConstants::DEATH_ANIMATION, 65);
    this->add_component<BehaviorScript>(std::make_unique<DyingCapybaraBehavior>(disappear_time_));
    this->add_component<Rigidbody2D>(BodyType2D::Dynamic, 30.0f, true, 3.0f);
    this->add_component<BoxCollider2D>(1.0f, 1.0f, 32.0f * size_modifier, 16.0f * size_modifier, Point{0.0f, 16.0f * size_modifier});
}