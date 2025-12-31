#include <game/character/gui/player_info_component.h>

#include <format>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/scene.h>
#include <engine/public/ui/ui_image.h>
#include <engine/public/ui/ui_object.h>
#include <engine/public/ui/ui_text.h>

#include <game/character/player_controller.h>
#include <game/character/gui/health_bar_component.h>
#include <game/character/gui/lives_bar_component.h>

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

    constexpr float life_width = 24;
    constexpr float life_height = 24;
    constexpr float lives_bar_internal_offset = life_width / 3;
    constexpr float lives_bar_height = life_height;
    constexpr float lives_bar_width = right_col_width - (right_col_internal_offset * 2);

    constexpr float health_bar_width = right_col_width - (right_col_internal_offset * 2);
    constexpr float health_bar_height = right_col_height - (right_col_internal_offset * 6) - lives_bar_height -
                                        player_name_height;

    constexpr float health_bar_offset_left = right_col_internal_offset;
    constexpr float health_bar_offset_top = right_col_internal_offset + player_name_height;
    constexpr float death_banner_width = right_col_width;
    constexpr float death_banner_height = player_name_height;

    constexpr float lives_bar_offset_left = right_col_internal_offset;
    constexpr float lives_bar_offset_top = health_bar_height + player_name_height + right_col_internal_offset * 2;

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
            player.user_name(),
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

        std::optional<std::reference_wrapper<PlayerController> > player_controller_opt{std::nullopt};
        for (auto &comp: player.get_components<BehaviorScript>()) {
            if (auto *controller = dynamic_cast<PlayerController *>(&comp.get().behavior())) {
                player_controller_opt.emplace(*controller);
            }
        }
        if (!player_controller_opt.has_value()) {
            throw std::runtime_error(
                "A player must have a PlayerController behavior registered in order to create a player_info gui element");
        }
        PlayerController &controller = player_controller_opt.value();

        right_col.add_child(player_name(scene, player));
        right_col.add_child(scene.add_game_object<HealthBar>(scene, controller, health_bar_width, health_bar_height,
                                                             health_bar_offset_left, health_bar_offset_top,
                                                             death_banner_width, death_banner_height));
        right_col.add_child(scene.add_game_object<LivesBar>(scene, controller, lives_bar_offset_left,
                                                            lives_bar_offset_top, life_width, life_height,
                                                            lives_bar_internal_offset));

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
