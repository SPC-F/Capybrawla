#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/sprite.h>
#include <engine/public/gameObject.h>
#include <engine/public/util/layers.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/animator.h>

#include <game/scenes/swamp.h>
#include <game/character/player_outofbounds_behavior.h>
#include <game/round/roundcontroller.h>
#include <game/character/player_object.h>
#include <game/prefabs/ai_drone_agent_object.h>
#include <game/scenes/level_loader.h>
#include <game/scripts/timer/RoundTimer.h>

#include <engine/public/components/colliders/box_collider_2d.h>
#include <game/behaviors/weapon_melee_behavior.h>
#include <engine/physics/physics_service.h>

const Vector3 DEFAULT_RESPAWN_POSITION = {600, 0, 0};

void load_players(Scene& scene, RoundController& controller, float start_x = 1000.0f, float start_y = 500.0f) {
    auto& player = scene.add_game_object<PlayerObject>(scene, Vector3{start_x, start_y, 0});

    player.add_component<BehaviorScript>(std::make_unique<PlayerOutOfBoundsBehavior>(
      -SwampScene::out_of_bounds_margin_x,
      SwampScene::map_width + SwampScene::out_of_bounds_margin_x,
      -SwampScene::out_of_bounds_margin_y,
      SwampScene::map_height + SwampScene::out_of_bounds_margin_y));

    // add base offset for proper alignment
    auto& melee_weapon = scene.add_game_object("Player_Melee_Weapon");
    melee_weapon.parent(player);

    // hitbox
    auto& melee_weapon_hitbox = scene.add_game_object("Player_Melee_Weapon_Hitbox");
    melee_weapon_hitbox.parent(melee_weapon);
    melee_weapon_hitbox.add_component<Rigidbody2D>(BodyType2D::Kinematic);
    melee_weapon_hitbox.add_component<BoxCollider2D>(0.5f, 0.1f, 32.0f, 50.0f, Point{0.0f, 0.0f}, true, false);

    // sprite
    auto& melee_weapon_sprite = scene.add_game_object("Player_Melee_Weapon_Sprite");
    melee_weapon_sprite.transform().scale({1.5f, 1.5f, 0.0f});
    melee_weapon_sprite.parent(melee_weapon);
    melee_weapon_sprite.add_component<Sprite>("bat", Color{255, 255, 255, 255}, 0, 0, 0, 0);
    melee_weapon_sprite.add_component<Animator>("bat_swing_anim", 30);
    melee_weapon_sprite.layer(Layers::Foreground + 1);

    // for animator change the position to show a proper swing position
    melee_weapon.add_component<BehaviorScript>(std::make_unique<WeaponMeleeBehavior>("bat_swing_anim", 10, 20, 1, 300.0f, melee_weapon_hitbox, melee_weapon_sprite, Point{60, 0}, Point{30.0f, 0}, Point{15.0f, 0}));

    player.layer(Layers::Foreground);

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

    auto& physics_service = engine.services->get_service<PhysicsService>().get();
    physics_service.debug_mode(true);

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

    return scene;
}