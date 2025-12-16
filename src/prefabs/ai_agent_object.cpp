#include <game/prefabs/ai_agent_object.h>

#include <engine/public/scene.h>
#include <engine/public/components/animator.h>
#include <engine/public/components/ai/ai_controller.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>

AIAgentObject::AIAgentObject(Scene &scene, const Vector3 initial_pos, std::vector<Vector3> target_positions)
    : GameObject(scene) {
  this->name("AIAgentObject");
  this->transform().position(initial_pos);

  constexpr float scale_factor = 1.0f;
  constexpr float size = 32.0f;

  this->transform().scale({scale_factor, scale_factor, 1.0f});

  this->add_component<Sprite>("", Color{0, 0, 255, 255}, 0, 0, 0, 0);
  this->add_component<Rigidbody2D>(BodyType2D::Dynamic, 30.0f, true, 1.0f);
  this->add_component<BoxCollider2D>(0.1f, 0.2f, size * scale_factor, size * scale_factor,
                                          Point{0.0f, 0.0f});

  std::string id;
  auto objects = scene.game_objects();
  for (auto& obj_ref : objects) {
      auto& obj = obj_ref.get();
      if (obj.name() == "TileMapDynamic_Parent") {
          id = obj.id();
          break;
      }
  }

  if (patrol_points_.empty()) {
    patrol_points_ = std::move(target_positions);
  }

  auto& tile_parent = scene.get_game_object(id).get();

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