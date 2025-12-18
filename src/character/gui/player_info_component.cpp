#include "engine/public/components/behaviorscript.h"
#include "engine/public/scene.h"
#include "engine/public/ui/ui_image.h"
#include "engine/public/ui/ui_object.h"
#include "engine/public/ui/ui_text.h"
#include "game/character/player_controller.h"

#include <format>
#include <game/character/gui/player_info_component.h>

namespace PlayerInfoComponent {
    constexpr float component_width = 350;
    constexpr float component_height = 100;

    constexpr float left_col_width = component_height;
    constexpr float left_col_height = component_height;

    constexpr float right_col_width = component_width - left_col_width;
    constexpr float right_col_height = component_height;
    constexpr float right_col_internal_offset = 5;

    constexpr float player_name_width = right_col_width;
    constexpr float player_name_height = 16;

    constexpr float live_width = 24;
    constexpr float live_height = 24;
    constexpr float lives_bar_internal_offset = live_width / 3;

    constexpr float health_bar_width = right_col_width - (right_col_internal_offset * 2);
    constexpr float health_bar_height = right_col_height - (right_col_internal_offset * 6) - live_height -
                                        player_name_height;

    HealthBar::HealthBar(Scene &scene, PlayerControllerBehavior &controller)
        : GameObject(scene),
          _health_changed_subscription{} {
        UIImage &health_bar = scene.add_game_object<UIImage>(scene, "", health_bar_width, health_bar_height, Point{},
                                                             Point{});
        health_bar.color(Color(255, 0, 0, 100));
        health_bar.transform().local_position({
            right_col_internal_offset, right_col_internal_offset + player_name_height, 0
        });
        this->add_child(health_bar);

        this->_health_changed_subscription = controller.on_health_changed(
            [&](const int old_health, const int new_health) {
                if (old_health > 0 && new_health <= 0 && controller.lives() < 1) {
                    scene.remove_game_object(health_bar);

                    UIText &skill_issue = scene.add_game_object<UIText>(
                        scene,
                        "Skill issue",
                        "ByteBounce",
                        "resources/fonts/bytebounce/ByteBounce.ttf",
                        right_col_width,
                        player_name_height,
                        Point{},
                        Point{});

                    skill_issue.transform().local_position({
                        right_col_internal_offset, right_col_internal_offset + player_name_height, 0
                    });
                    this->add_child(skill_issue);
                    return;
                }

                if (new_health == old_health || new_health < 0) {
                    return;
                }

                if (new_health == controller.max_health()) {
                    scene.remove_game_object(health_bar);

                    UIImage &new_health_bar = scene.add_game_object<UIImage>(
                        scene, "", health_bar_width, health_bar_height, Point{}, Point{});
                    new_health_bar.color(Color(255, 0, 0, 100));
                    new_health_bar.transform().local_position({
                        right_col_internal_offset, right_col_internal_offset + player_name_height, 0
                    });
                    this->add_child(new_health_bar);

                    return;
                }

                const float health_percentage = static_cast<float>(new_health) / static_cast<float>(controller.
                                                    max_health());

                scene.remove_game_object(health_bar);

                UIImage &new_health_bar = scene.add_game_object<UIImage>(
                    scene, "", health_bar_width * health_percentage, health_bar_height, Point{}, Point{});
                new_health_bar.color(Color(255, 0, 0, 100));
                new_health_bar.transform().local_position({
                    right_col_internal_offset, right_col_internal_offset + player_name_height, 0
                });
                this->add_child(new_health_bar);
            });
    }
    LivesBar::LivesBar(Scene &scene,
                           PlayerControllerBehavior &controller)
            : GameObject(scene), _lives_changed_subscription{} {
        this->transform().local_position({
            right_col_internal_offset, health_bar_height + player_name_height + right_col_internal_offset * 2, 0
        });

        for (int i = 0; i < controller.lives(); i++) {
            UIImage &life_image = scene.add_game_object<UIImage>(scene, "heart", live_width, live_height, Point{},
                                                                 Point{});
            life_image.transform().local_position({i * (live_width + lives_bar_internal_offset), 0, 0});
            this->add_child(life_image);
        }

        _lives_changed_subscription = controller.on_lives_changed(
            [&](const int old_lives_count, const int new_lives_count) {
                if (!controller.is_alive() && old_lives_count < 1 && new_lives_count < 1) {
                    return;
                }

                const auto children_copy = this->children();
                for (auto &child: children_copy) {
                    scene.remove_game_object(child);
                }

                for (int i = 0; i < controller.lives(); i++) {
                    UIImage &life_image = scene.add_game_object<UIImage>(
                        scene, "heart", live_width, live_height, Point{},
                        Point{});
                    life_image.transform().local_position({i * (live_width + lives_bar_internal_offset), 0, 0});
                    this->add_child(life_image);
                }
            });
    }

    GameObject &left_col(Scene &scene, PlayerObject &player) {
        GameObject &left_col = scene.add_game_object("");

        auto &player_skin_image = scene.add_game_object<UIImage>(scene, "", left_col_width, left_col_height, Point{},
                                                                 Point{});
        player_skin_image.color({100, 255, 100, 100});
        left_col.add_child(player_skin_image);

        return left_col;
    }

    UIText &player_name(Scene &scene, const PlayerObject &player) {
        UIText &player_name = scene.add_game_object<UIText>(
            scene,
            player.name(),
            "ByteBounce",
            "resources/fonts/bytebounce/ByteBounce.ttf",
            right_col_width,
            player_name_height,
            Point{},
            Point{});

        player_name.transform().local_position({right_col_internal_offset, right_col_internal_offset, 0});
        return player_name;
    }

    GameObject &right_col(Scene &scene, const PlayerObject &player) {
        GameObject &right_col = scene.add_game_object("");
        right_col.transform().local_position({left_col_width, 0, 0});

        std::optional<std::reference_wrapper<PlayerControllerBehavior> > player_controller_opt{std::nullopt};
        for (auto &comp: player.get_components<BehaviorScript>()) {
            if (auto *controller = dynamic_cast<PlayerControllerBehavior *>(&comp.get().behavior())) {
                player_controller_opt.emplace(*controller);
            }
        }
        if (!player_controller_opt.has_value()) {
            throw std::runtime_error(
                "A player must have a PlayerController behavior registered in order to create a player_info gui element");
        }
        PlayerControllerBehavior &controller = player_controller_opt.value();

        right_col.add_child(player_name(scene, player));
        right_col.add_child(scene.add_game_object<HealthBar>(scene, controller));
        right_col.add_child(scene.add_game_object<LivesBar>(scene, controller));

        return right_col;
    }

    GameObject &create_and_add(Scene &scene, PlayerObject &player) {
        GameObject &gui_component = scene.add_game_object(std::format("{}-info", player.name()));

        UIImage &background = scene.add_game_object<UIImage>(scene, "", component_width, component_height, Point{},
                                                             Point{});
        background.color({100, 100, 100, 100});

        gui_component.add_child(background);

        gui_component.add_child(left_col(scene, player));
        gui_component.add_child(right_col(scene, player));

        return gui_component;
    }
}
