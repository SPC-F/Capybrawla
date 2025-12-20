#include <game/behaviors/ui/main_menu/main_menu_fall_behavior.h>

MainMenuFallBehavior::MainMenuFallBehavior(float rot_speed, float fall_speed, float lowest_point) 
    : rotation_speed_(rot_speed), fall_speed_(fall_speed), lowest_point_(lowest_point) {}
    
void MainMenuFallBehavior::on_update(float dt) {
    auto& transform = this->transform();
        
    Vector3 position = transform.position();
    position.y += fall_speed_ * dt;

    float new_rotation = transform.rotation() + rotation_speed_ * dt;
    transform.rotation(new_rotation);

    transform.position(position);

    if (position.y > lowest_point_ + 10.0f) {
        this->game_object().mark_for_deletion();
    }
}
