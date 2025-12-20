#include <game/character/gui/health_bar_component.h>

namespace PlayerInfoComponent {
    HealthBar::HealthBar(Scene &scene,
                         PlayerController &controller,
                         float health_bar_width,
                         float health_bar_height,
                         float health_bar_offset_left,
                         float health_bar_offset_top,
                         float death_banner_width,
                         float death_banner_height)
        : GameObject(scene),
          _health_changed_subscription{} {
        UIImage &health_bar = scene.add_game_object<UIImage>(scene, "", health_bar_width, health_bar_height,
                                                             Point{}, Point{});
        health_bar.color(Color(255, 0, 0, 100));
        health_bar.transform().local_position({
            health_bar_offset_left, health_bar_offset_top, 0
        });
        this->add_child(health_bar);

        UIText &death_text = scene.add_game_object<UIText>(
            scene,
            "",
            "ByteBounce",
            "resources/fonts/bytebounce/ByteBounce.ttf",
            death_banner_width,
            death_banner_height,
            Point{},
            Point{});

        death_text.transform().local_position({
            health_bar_offset_left, health_bar_offset_top, 0
        });

        this->add_child(death_text);

        this->_health_changed_subscription = controller.on_health_changed(
            [=, &health_bar, &death_text, &controller](const int old_health, const int new_health) {
                if (controller.is_hard_dead()) {
                    health_bar.width(0);
                    death_text.text("Unfortunate skill issue");
                    return;
                }
                if (!controller.is_hard_dead() && !death_text.text().empty()) {
                    death_text.text("");
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
