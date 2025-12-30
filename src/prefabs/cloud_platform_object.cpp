#include <game/prefabs/cloud_platform_object.h>

#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/layers.h>

#include <game/behaviors/cloud_platform_behavior.h>

constexpr float scale_factor = 3.0f;

constexpr float width = 32.0f;
constexpr float height = 16.0f;

CloudPlatformObject::CloudPlatformObject(Scene& scene, const Vector3 position)
    : GameObject(scene) {
    this->name("Cloud Platform");
    this->tag("Ground");
    this->layer(Layers::Background + 2);

    this->transform().position(position);
    
    this->transform().scale({scale_factor, scale_factor, 1.0f});

    this->add_component<Sprite>("cloud_platform", Color{255, 255, 255, 255}, 0, 0, 0, 0);
    this->add_component<Rigidbody2D>(BodyType2D::Static, 0.0f, true, 1.0f);
    this->add_component<BoxCollider2D>(0.0f, 0.0f, width * scale_factor, height * scale_factor, Point{0.0f, 0.0f});
    this->add_component<BehaviorScript>(std::make_unique<CloudPlatformBehavior>());
}