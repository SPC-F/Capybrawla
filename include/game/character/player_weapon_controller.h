#pragma once

#include <engine/public/components/behaviorscript.h>

class PlayerWeaponController final : public Behavior {
private:
  std::reference_wrapper<GameObject> default_weapon_;
  std::optional<std::reference_wrapper<GameObject>> found_weapon_;

public:
  explicit PlayerWeaponController(GameObject& weapon);
  ~PlayerWeaponController() override = default;

  void on_start() override;
  void on_update(float dt) override;

  void switch_weapon(std::optional<std::reference_wrapper<GameObject>> new_weapon);
  void drop_found_weapon();
};