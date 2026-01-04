#include <game/character/gui/health_bar_component.h>

#include <engine/public/ui/ui_image.h>
#include <engine/public/ui/ui_text.h>



namespace PlayerInfoComponent {
    HealthBar::HealthBar(Scene &scene,
                         PlayerController &controller,
                         float health_bar_width,
                         float health_bar_height,
                         float health_bar_offset_left,
                         float health_bar_offset_top)
        : GameObject(scene),
          _health_changed_subscription{} {
        UIImage &health_bar_red = scene.add_game_object<UIImage>(scene, "", health_bar_width, health_bar_height,
                                                             Point{}, Point{});
        health_bar_red.color(Color(217, 87, 99, 255));
        health_bar_red.transform().local_position({
            health_bar_offset_left, health_bar_offset_top, 0
        });
        this->add_child(health_bar_red);

        UIImage &health_bar = scene.add_game_object<UIImage>(scene, "", health_bar_width, health_bar_height,
                                                             Point{}, Point{});
        health_bar.color(Color(147, 215, 110, 255));
        health_bar.transform().local_position({
            health_bar_offset_left, health_bar_offset_top, 0
        });
        this->add_child(health_bar);
        
        UIImage &health_bar_frame = scene.add_game_object<UIImage>(scene, "health_frame", health_bar_width, health_bar_height * 2,
                                                             Point{}, Point{});

        health_bar_frame.transform().local_position({
            health_bar_offset_left, health_bar_offset_top, 0
        });
        this->add_child(health_bar_frame);

        this->_health_changed_subscription = controller.on_health_changed(
            [=, &health_bar, &controller](const int old_health, const int new_health) {
                if (controller.is_hard_dead()) {
                    health_bar.width(0);
                    return;
                }

                if (new_health == old_health || new_health < 0) {
                    return;
                }

                if (new_health == controller.max_health()) {
                    health_bar.width(health_bar_width);
                    return;
                }

                const float health_percentage = static_cast<float>(new_health) / static_cast<float>(controller.
                                                    max_health());

                health_bar.width(health_bar_width * health_percentage);
            });
    }
}
