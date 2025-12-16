#include <game/prefabs/ai_agent_object.h>

#include <game/behaviors/ai_drone_movement_behavior.h>
#include <game/behaviors/ai_rendering_behavior.h>

#include <engine/public/scene.h>
#include <engine/public/components/animator.h>
#include <engine/public/components/ai/ai_controller.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>

GameObject& get_tilemap_parent(Scene& scene) {
  std::string id;
  auto objects = scene.game_objects();
  for (auto& obj_ref : objects) {
    auto& obj = obj_ref.get();
    if (obj.name() == "TileMapDynamic_Parent") {
      id = obj.id();
      break;
    }
  }

  return scene.get_game_object(id).get();
}

AIAgentObject::AIAgentObject(Scene &scene, const Vector3 initial_pos, std::vector<Vector3> target_positions)
    : GameObject(scene) {
  this->name("AIAgentObject");
  this->transform().position(initial_pos);

  auto& tile_parent = get_tilemap_parent(scene);
  
  constexpr float scale_factor = 1.5f;
  constexpr float size = 32.0f;
  constexpr float margin = 4.0f;

  this->transform().scale({scale_factor, scale_factor, 1.0f});

  this->add_component<Sprite>("drone_idle", Color{255, 255, 255, 255}, 0, 0, 0, 0);
  this->add_component<Rigidbody2D>(BodyType2D::Dynamic, 30.0f, true, 1.0f);
  this->add_component<BoxCollider2D>(0.1f, 0.2f, (size - margin) * scale_factor, (size - 6.0f) * scale_factor,
                                          Point{2.0f, 8.0f});
  this->add_component<Animator>("drone_idle_anim", 64).play(true);
  this->add_component<BehaviorScript>(std::make_unique<AIDroneMovementBehavior>());
  this->add_component<BehaviorScript>(std::make_unique<AIRenderingBehavior>(std::ref(tile_parent)));
  
  if (patrol_points_.empty()) patrol_points_ = std::move(target_positions);

  auto& ai_controller = this->add_component<AIController>(tile_parent)
                              .set_height(size * scale_factor)
                              .set_width(size * scale_factor);
  ai_controller.set_arrival_threshold(size * scale_factor);

  patrol_target_ = scene.add_game_object("AIAgent_PatrolTarget");
  patrol_target_->get().transform().position(patrol_points_[location_index_]);

  ai_controller.set_patrol_target(*patrol_target_);
  ai_controller.set_mode(AIControllerMode::PATROL);

  ai_controller.add_on_patrol_complete_action([this](AIController& ctrl) {
      if (patrol_points_.empty()) return;
      
      location_index_ = (location_index_ + 1) % patrol_points_.size();
      patrol_target_->get().transform().position(patrol_points_[location_index_]);
  });
}