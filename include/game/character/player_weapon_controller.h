#pragma once

#include <engine/public/components/behaviorscript.h>
#include <engine/public/scene.h>

class PlayerWeaponController final : public Behavior {
private:
  std::reference_wrapper<GameObject> default_weapon_;
  std::optional<std::reference_wrapper<GameObject>> found_weapon_;

public:
  explicit PlayerWeaponController(GameObject& weapon);
  ~PlayerWeaponController() override = default;

  void on_update(float dt) override;

  template<typename T>
  void switch_weapon() {   
    drop_found_weapon();

    /// Create a new weapon object and feed it the player as the found weapon
    found_weapon_ = game_object().scene().add_game_object<T>(game_object().scene(), game_object());
    found_weapon_->get().set_active();
    
    default_weapon_.get().set_inactive();
  }

  void destroy_found_weapon();
  void drop_found_weapon();
  void send_drop_weapon_message();

  bool has_found_weapon() const;
};