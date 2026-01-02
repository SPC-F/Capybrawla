#include <game/scenes/swamp.h>

#include <game/character/gui/player_info_component.h>
#include <game/behaviors/interactable/ItemDropper.h>
#include <game/character/player_outofbounds_behavior.h>
#include <game/character/player_object.h>
#include <game/prefabs/ai_drone_agent_object.h>
#include <game/prefabs/cloud_platform_object.h>
#include <game/prefabs/config/health_pack_config.h>
#include <game/prefabs/config/weapon_bat_config.h>
#include <game/prefabs/config/weapon_sword_config.h>
#include <game/prefabs/config/weapon_axe_config.h>
#include <game/round/roundcontroller.h>
#include <game/scenes/swamp.h>
#include <game/scripts/timer/RoundTimer.h>

#include <engine/audio/audio_service.h>
#include <engine/core/engine.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/layers.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/animator.h>

SwampScene::SwampScene() : Level("Level_SwampScene") {}

void SwampScene::load_players(Scene& scene, RoundController& controller, float start_x, float start_y) {
    auto& player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x, start_y, 0});

    player.add_component<BehaviorScript>(std::make_unique<PlayerOutOfBoundsBehavior>(
      -out_of_bounds_margin_x_,
      map_width_ + out_of_bounds_margin_x_,
      -out_of_bounds_margin_y_,
      map_height_ + out_of_bounds_margin_y_));

    player.layer(Layers::Foreground);
    player.set_controllable();
    player.user_name("Real player");

    auto& ai_player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x + 40.0f, 100.0f, 0}, false);
    ai_player.user_name("AI Player");

    controller.add_player(player);
    controller.add_player(ai_player);
}

RoundController& SwampScene::add_round_controller(Scene& scene) {
    std::vector<Vector3> respawn_positions = {
        Vector3{200.0f, 100.0f, 0.0f},
        Vector3{1160.0f, 100.0f, 0.0f},
        Vector3{1600.0f, 100.0f, 0.0f},
    };

    GameObject& wrapper = scene.add_game_object("RoundControllerWrapper");
    auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<RoundController>(respawn_positions));
    return *dynamic_cast<RoundController*>(&comp.behavior());
}

void SwampScene::load_ai_agent(Scene& scene) {
    std::vector<Vector3> patrol_points = {
        Vector3{1400.0f, 300.0f, 0.0f},
        Vector3{380.0f, 400.0f, 0.0f},
    };

    Vector3 initial_position = Vector3{1000.0f, 600.0f, 0.0f};
    scene.add_game_object<AIDroneAgentObject>(scene, initial_position, patrol_points);
}

void SwampScene::load_timer(Scene& scene) {
    GameObject& round_timer_obj = scene.add_game_object("Round Timer");

    auto round_timer = std::make_unique<RoundTimer>();
    round_timer->start_timer([] {});
    round_timer_obj.add_component<BehaviorScript>(std::move(round_timer));
}

void SwampScene::load_interactables(Scene& scene) {
    std::vector<std::pair<float, float>> positions;
    positions.emplace_back(1368, 480);
    positions.emplace_back(350, 432);

    for (auto pos : positions) {
        std::vector<std::unique_ptr<PrefabRegistrable>> drops;
        drops.emplace_back(std::make_unique<HealthPackConfig>());
        drops.emplace_back(std::make_unique<WeaponAxeConfig>());
        drops.emplace_back(std::make_unique<WeaponBatConfig>());
        drops.emplace_back(std::make_unique<WeaponSwordConfig>());

        auto& obj = scene.add_game_object("interactable_spawner");
        obj.add_component<BehaviorScript>(std::make_unique<ItemDropper>(std::move(drops)));
        obj.add_component<Sprite>("item_dropper", Color(), 0, 0, 0, 0);
        obj.add_component<Animator>("item_dropper_idle", 128).play(true);
        obj.transform().scale({2, 2, 2});
        obj.transform().position({pos.first, pos.second, 0});
    }
}

void SwampScene::setup(Scene& scene) {
    AudioService &audio_service = Engine::instance().services->get_service<AudioService>().get();
    scene.on_run([&audio_service](Scene& scene) {
        audio_service.play_sound("spear_of_justice", 0.05f, true);
    });

    scene.on_stop([&audio_service](Scene& scene) {
        audio_service.stop_all_sounds();
    });
}

void SwampScene::load(Scene& scene) {
    SwampScene::load_camera();
    SwampScene::load_map(std::string(Assets::MAP_SWAMP));

    load_timer(scene);

    RoundController& controller = add_round_controller(scene);
    load_players(scene, controller, 1000.0f, 500.0f);
    load_ai_agent(scene);
    load_interactables(scene);
}