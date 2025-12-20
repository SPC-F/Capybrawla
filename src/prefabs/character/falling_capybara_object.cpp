#include <game/prefabs/character/falling_capybara_object.h>
#include <game/behaviors/ui/main_menu/main_menu_fall_behavior.h>

#include <engine/public/scene.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/behaviorscript.h>

FallingCapybaraObject::FallingCapybaraObject(Scene& scene, float max_fall_height, float size_modifier, float rotation_speed, float fall_speed, float position_x, float position_y)
: GameObject(scene),
  max_fall_height_{max_fall_height},
  size{size_modifier},
  rotation_speed{rotation_speed},
  fall_speed{fall_speed},
  position_x{position_x},
  position_y{position_y} 
  {
    this->name("Falling_Capybara_Object");
    this->layer(Layers::Foreground);
    this->transform().scale({size_modifier, size_modifier, 1.0f});
    this->transform().position({position_x, position_y, 0.0f});
    
    float capybara_choice   = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 5.0f));
    std::string chosen_capybara = capybara_variants_[static_cast<int>(capybara_choice) % capybara_variants_.size()];
    
    this->add_component<Sprite>(chosen_capybara, Color{255, 255, 255, 255}, 0, 0, 0, 0);
    this->add_component<BehaviorScript>(std::make_unique<MainMenuFallBehavior>(rotation_speed, fall_speed, static_cast<float>(max_fall_height_)));
    this->layer(Layers::Foreground);

}