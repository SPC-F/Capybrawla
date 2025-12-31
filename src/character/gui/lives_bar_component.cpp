#include <game/character/gui/lives_bar_component.h>
#include <functional>

namespace PlayerInfoComponent {

    LivesBar::LivesBar(Scene &scene,
                       PlayerController &controller,
                       const float lives_bar_offset_left,
                       const float lives_bar_offset_top,
                       const float life_width,
                       const float life_height,
                       const float lives_bar_internal_offset)
        : GameObject(scene), _lives_changed_subscription{} {

        this->transform().local_position({lives_bar_offset_left, lives_bar_offset_top, 0});

        const int max_lives = controller.lives();
        life_images_.reserve(max_lives);

        for (int i = 0; i < max_lives; i++) {
            UIImage &life_image = scene.add_game_object<UIImage>(
                scene, "heart", life_width, life_height, Point{}, Point{});
            life_image.transform().local_position({i * (life_width + lives_bar_internal_offset), 0, 0});
            this->add_child(life_image);

            if (i < controller.lives()) {
                life_image.set_active();
            } else {
                life_image.set_inactive();
            }
              // Only activate starting lives
            life_images_.push_back(std::ref(life_image));  // store as reference wrapper
        }

        _lives_changed_subscription = controller.on_lives_changed(
            [this, &controller](const int /*old_lives*/, const int new_lives) {
                const int max_lives = static_cast<int>(life_images_.size());
                for (int i = 0; i < max_lives; i++) {
                    if (i < controller.lives()) {
                        life_images_[i].get().set_active();
                    } else {
                        life_images_[i].get().set_inactive();
                    }
                }
            });
    }

} // namespace PlayerInfoComponent
