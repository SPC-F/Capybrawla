#include "game/character/gui/player_info_component.h"
#include <game/scenes/swamp.h>

#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/audio/audio_service.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/layers.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/animator.h>

#include <game/character/player_outofbounds_behavior.h>
#include <game/character/player_object.h>
#include <game/prefabs/ai_drone_agent_object.h>
#include <game/prefabs/weapons/weapon_bat_player_object.h>
#include <game/prefabs/weapons/weapon_axe_player_object.h>
#include <game/prefabs/weapons/weapon_sword_player_object.h>
#include <game/round/roundcontroller.h>
#include <game/scenes/level_loader.h>
#include <game/scenes/swamp.h>
#include <game/scripts/timer/RoundTimer.h>

const Vector3 DEFAULT_RESPAWN_POSITION = {600, 0, 0};

void load_players(Scene& scene, RoundController& controller, float start_x = 1000.0f, float start_y = 500.0f) {
    auto& player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x, start_y, 0});

    player.add_component<BehaviorScript>(std::make_unique<PlayerOutOfBoundsBehavior>(
      -SwampScene::out_of_bounds_margin_x,
      SwampScene::map_width + SwampScene::out_of_bounds_margin_x,
      -SwampScene::out_of_bounds_margin_y,
      SwampScene::map_height + SwampScene::out_of_bounds_margin_y));

    auto& weapon_axe = scene.add_game_object<WeaponAxePlayerObject>(scene, player);

    player.layer(Layers::Foreground);
    player.set_controllable();

    GameObject& player_info_comp = PlayerInfoComponent::create_and_add(scene, player);
    player_info_comp.transform().position({50, 950, 0});

    auto& ai_player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x + 40.0f, 100.0f, 0}, false);

    controller.add_player(player);
    controller.add_player(ai_player);
}

RoundController& add_round_controller(Scene& scene) {
    GameObject& wrapper = scene.add_game_object("RoundControllerWrapper");
    auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<RoundController>(DEFAULT_RESPAWN_POSITION));
    return *dynamic_cast<RoundController*>(&comp.behavior());
}

void load_ai_agent(Scene& scene) {
    std::vector<Vector3> patrol_points = {
        Vector3{1400.0f, 300.0f, 0.0f},
        Vector3{380.0f, 400.0f, 0.0f},
    };

    Vector3 initial_position = Vector3{1000.0f, 600.0f, 0.0f};
    scene.add_game_object<AIDroneAgentObject>(scene, initial_position, patrol_points);
}

void load_timer(Scene& scene) {
    GameObject& round_timer_obj = scene.add_game_object("Round Timer");
    auto round_timer = std::make_unique<RoundTimer>();
    round_timer->start_timer([] {});
    round_timer_obj.add_component<BehaviorScript>(std::move(round_timer));
}

Scene& SwampScene::setup() {
    const Engine& engine = Engine::instance();

    RenderingService& rendering_service = Engine::instance().services->get_service<RenderingService>().get();
    const int window_width = rendering_service.window().get_window_width();
    const int window_height = rendering_service.window().get_window_height();

    Scene& scene = engine.services->get_service<SceneService>().get().add_scene(SCENE_NAME);
    auto& camera = scene.add_game_object<Camera>(scene, Color(), 1.0f, true);
    camera.transform().position({
        static_cast<float>(window_width) / 2.0f,
        static_cast<float>(window_height) / 2.0f, 0.0f});

    load_timer(scene);

    LevelLoader loader;
    loader.load_game_objects_from_json(
        std::string(Assets::MAP_SWAMP),
        scene
    );

    RoundController& controller = add_round_controller(scene);
    load_players(scene, controller);
    load_ai_agent(scene);

    AudioService &audio_service = Engine::instance().services->get_service<AudioService>().get();
    scene.on_run([&audio_service](Scene& scene) {
        audio_service.play_sound("spear_of_justice", 0.1f, true);
    });

    scene.on_stop([&audio_service](Scene& scene) {
        audio_service.stop_all_sounds();
    });

    return scene;
}