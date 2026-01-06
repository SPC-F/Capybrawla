#include <game/prefabs/ui/end_round_result_object.h>

#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/sprite.h>
#include <engine/public/ui/ui_text.h>
#include <engine/public/util/layers.h>

#include <game/behaviors/cloud_platform_behavior.h>
#include <game/behaviors/ui/end_round_continue_behavior.h>

constexpr float scale_factor = 3.0f;

constexpr float width = 32.0f;
constexpr float height = 16.0f;

EndRoundResultObject::EndRoundResultObject(Scene& scene, std::optional<std::reference_wrapper<PlayerObject>> player_opt, bool is_draw)
    : GameObject(scene) {
    this->name("End Round Result");
    this->tag("UI");
    this->layer(Layers::UI + 1);
    
    float screen_width = static_cast<float>(scene.main_camera()->get().get_screen_width());
    float screen_height = static_cast<float>(scene.main_camera()->get().get_screen_height());
    this->transform().scale({screen_width, screen_height, 1});
    this->add_component<Sprite>("", Color(0, 0, 0, 150), 0, 0, false, false);

    std::string draw_text = "Draw!";
    std::string winner_text = "Winner: " + (player_opt.has_value() ? player_opt->get().user_name() : "Capybara");
    std::string display_text = is_draw ? draw_text : winner_text;

    float text_width = 400.0f;
    float text_height = 200.0f;
    auto& text = scene.add_game_object<UIText>(
        scene,
        display_text,
        "ByteBounce",
        "resources/fonts/bytebounce/ByteBounce.ttf",
        text_width,
        text_height,
        Point{0.5f, 0.5f},
        Point{0.5f, 0.5f}
    );

    text.parent(*this);
    text.font_size(116);
    text.layer(Layers::UI + 2);
    text.transform().position({
        (screen_width - text_width) / 2.0f,
        (screen_height - text_height) / 2.0f - 80.0f,
        0.0f
    });

    std::string capybara_sprite = player_opt.has_value() ? player_opt.value().get().constants().death_animation.data() : "capybara_default_duck_anim";
    auto& capybara = scene.add_game_object("WinningCapybara");
    capybara.parent(*this);
    capybara.add_component<Sprite>(capybara_sprite, Color(), 0, 0, 0, 0);
    capybara.transform().scale({4.0f, 4.0f, 1.0f});

    float capybara_width = 32.0f * 4.0f;
    float capybara_height = 32.0f * 4.0f;
    capybara.transform().position({
        (screen_width - capybara_width) / 2.0f,
        (screen_height + text_height) / 2.0f + 20.0f,
        0.0f
    });
    capybara.layer(Layers::UI + 3);

    this->add_component<BehaviorScript>(std::make_unique<EndRoundContinueBehavior>());
}