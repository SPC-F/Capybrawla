#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/layers.h>
#include <engine/public/components/behaviorscript.h>

#include <game/scenes/swamp.h>
#include <game/character/player_outofbounds_behavior.h>
#include <game/round/roundcontroller.h>
#include <game/character/player_object.h>
#include <game/scenes/level_loader.h>
#include <game/scripts/timer/RoundTimer.h>

const Vector3 DEFAULT_RESPAWN_POSITION = {600, 0, 0};

void load_players(Scene& scene, RoundController& controller, float start_x = 1000.0f, float start_y = 500.0f) {
    auto& player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x, start_y, 0});

    player.add_component<BehaviorScript>(std::make_unique<PlayerOutOfBoundsBehavior>(
      -SwampScene::out_of_bounds_margin_x,
      SwampScene::map_width + SwampScene::out_of_bounds_margin_x,
      -SwampScene::out_of_bounds_margin_y,
      SwampScene::map_height + SwampScene::out_of_bounds_margin_y));

    player.layer(Layers::Foreground);
    controller.add_player(player);
}

RoundController& add_round_controller(Scene& scene) {
    GameObject& wrapper = scene.add_game_object("RoundControllerWrapper");
    auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<RoundController>(DEFAULT_RESPAWN_POSITION));
    return *dynamic_cast<RoundController*>(&comp.behavior());
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

    GameObject& round_timer_obj = scene.add_game_object("Round Timer");
    auto round_timer = std::make_unique<RoundTimer>();
    round_timer->start_timer([] {});
    round_timer_obj.add_component<BehaviorScript>(std::move(round_timer));
    
    LevelLoader loader;
    loader.load_game_objects_from_json(
        std::string(Assets::MAP_SWAMP),
        scene
    );

    RoundController& controller = add_round_controller(scene);
    load_players(scene, controller);

    return scene;
}