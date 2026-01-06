#include <engine/public/components/behaviorscript.h>

#include <game/behaviors/interactable/ItemDropper.h>
#include <game/behaviors/multiplayer/multiplayer_controller.h>
#include <game/behaviors/weapon_melee_behavior.h>
#include <game/character/player_outofbounds_behavior.h>
#include <game/character/player_weapon_controller.h>
#include <game/prefabs/config/weapon_bat_config.h>
#include <game/prefabs/config/weapon_sword_config.h>
#include <game/prefabs/config/weapon_axe_config.h>
#include <game/prefabs/config/health_pack_config.h>
#include <game/prefabs/ui/end_round_result_object.h>
#include <game/round/roundcontroller.h>
#include <game/scenes/swamp_autum.h>
#include <game/scenes/level_loader.h>

#include <engine/public/util/layers.h>
#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/prefab_service.h>
#include <engine/network/multiplayer_service.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/components/sprite.h>
#include <engine/public/scene_service.h>
#include <engine/public/gameObject.h>
#include <engine/public/ui/ui_text.h>
#include <engine/util/uuid.h>

#include <iostream>

SwampAutumScene::SwampAutumScene() : Level("Level_SwampAutumScene") {}

void SwampAutumScene::load_interactables(Scene& scene) {
    std::vector<std::pair<float, float>> positions;
    positions.emplace_back(1368, 480);
    positions.emplace_back(350, 432);

    for (auto pos : positions) {
        auto& obj = create_interactable_dropper(uuid::generate_uuid_v4());
        obj.transform().position({pos.first, pos.second, 0});
    }
}

PlayerObject& SwampAutumScene::create_player_object(const std::string& name) {
    float start_x = 1000.0f;
    float start_y = 500.0f;

    auto& obj = scene().add_game_object<PlayerObject>(scene(), Vector3{start_x, start_y, 0});
    obj.prefab_type_id("PlayerObject");
    obj.add_component<BehaviorScript>(std::make_unique<PlayerOutOfBoundsBehavior>(
      -out_of_bounds_margin_x_,
      map_width_ + out_of_bounds_margin_x_,
      -out_of_bounds_margin_y_,
      map_height_ + out_of_bounds_margin_y_));

    Engine& engine = Engine::instance();
    auto& multiplayer_service = engine.services->get_service<MultiplayerService>().get();
    
    obj.add_component<NetworkIdentity>(name.c_str());
    obj.layer(Layers::Foreground);

    return obj;
}

GameObject& SwampAutumScene::create_interactable_dropper(const std::string& name) {
    auto& obj = scene().add_game_object("interactable_spawner");
    obj.prefab_type_id("InteractableDropper");

    std::vector<std::unique_ptr<PrefabRegistrable>> drops;
    drops.emplace_back(std::make_unique<HealthPackConfig>());
    drops.emplace_back(std::make_unique<WeaponAxeConfig>());
    drops.emplace_back(std::make_unique<WeaponBatConfig>());
    drops.emplace_back(std::make_unique<WeaponSwordConfig>());
        
    obj.add_component<BehaviorScript>(std::make_unique<ItemDropper>(std::move(drops)));
    obj.add_component<Sprite>("item_dropper", Color(), 0, 0, 0, 0);
    obj.add_component<Animator>("item_dropper_idle", 128).play(true);
    obj.transform().scale({2, 2, 2});

    Engine& engine = Engine::instance();
    auto& multiplayer_service = engine.services->get_service<MultiplayerService>().get();
    obj.add_component<NetworkIdentity>(name.c_str()).mark_dirty();

    return obj;
}

RoundController& SwampAutumScene::add_multiplayer_round_controller() {
    std::vector<Vector3> respawn_positions = {
        Vector3{200.0f, 100.0f, 0.0f},
        Vector3{1160.0f, 100.0f, 0.0f},
        Vector3{1600.0f, 100.0f, 0.0f},
    };

    GameObject& wrapper = scene().add_game_object("RoundControllerWrapper");
    
    auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<RoundController>(respawn_positions));
    auto& controller = *dynamic_cast<RoundController*>(&comp.behavior());

    return controller;
}

// MultiplayerController& SwampAutumScene::add_multiplayer_controller() {
//     GameObject& wrapper = scene().add_game_object("MultiplayerControllerWrapper");
//     auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<MultiplayerController>());
//     return *dynamic_cast<MultiplayerController*>(&comp.behavior());
// }

std::optional<std::reference_wrapper<PlayerObject>> SwampAutumScene::get_network_player_object(const std::string& uuid) {
    for (auto& game_object_ref : scene().game_objects()) {
        auto& game_object = game_object_ref.get();

        auto network_id_opt = game_object.get_component<NetworkIdentity>();
        if (!network_id_opt)
            continue;

        if (network_id_opt->get().uuid() == uuid) {
            if (auto* player = dynamic_cast<PlayerObject*>(&game_object)) {
                return *player;
            }
        }
    }

    return std::nullopt;
}

void SwampAutumScene::handle_player_movement_update(const MsgUserMove& data) {
    auto player_opt = get_network_player_object(data.uuid);
    if (!player_opt.has_value()) return;

    auto& player = player_opt.value().get();

    auto behaviors = player.get_components<BehaviorScript>();
    for (auto& behavior_ref : behaviors) {
        auto& behavior = behavior_ref.get().behavior();

        if (auto movement_behavior = dynamic_cast<PlayerMovementBehavior*>(&behavior); movement_behavior != nullptr) {
            std::vector<PlayerMovementTypes> movement;
            for (int i = 0; i < data.size; ++i) {
                movement.emplace_back(static_cast<PlayerMovementTypes>(data.movement[i]));
            }
            movement_behavior->handle_movement(movement);
        }
    }
}

void SwampAutumScene::handle_player_attack(const MsgUserAttack& data) {
    auto player_opt = get_network_player_object(data.uuid);
    if (!player_opt.has_value()) return;

    auto& player = player_opt.value().get();

    for (auto& child : player.children()) {
        if (child.get().tag() != "Weapon" || !child.get().is_active()) continue;
        auto melee_behavior_opt = child.get().get_script<BehaviorScript, WeaponMeleeBehavior>();

        if (melee_behavior_opt.has_value()) {
            melee_behavior_opt->get().attack();
            return;
        }
    }
}

void SwampAutumScene::handle_player_drop_weapon(const MsgUserDropWeapon& data) {
    auto player_opt = get_network_player_object(data.uuid);
    if (!player_opt.has_value()) return;

    auto& player = player_opt.value().get();

    auto weapon_controller_opt = player.get_script<BehaviorScript, PlayerWeaponController>();
    if (!weapon_controller_opt.has_value()) return;

    auto& weapon_controller = weapon_controller_opt->get();
    weapon_controller.drop_found_weapon();
}

void SwampAutumScene::setup(Scene& scene) {
    add_on_stop_callback([](Scene& scene) {
        auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();
        multiplayer_service.disconnect();
    });

    PrefabService& prefab_service = Engine::instance().services->get_service<PrefabService>().get();
    prefab_service.register_prefab("PlayerObject", [this](Scene& scene, const std::string& name) -> GameObject& {
        return create_player_object(name);
    });
    prefab_service.register_prefab("InteractableDropper", [this](Scene& scene, const std::string name) -> GameObject& {
        return create_interactable_dropper(name);
    });

    std::unique_ptr<PrefabRegistrable> registrables[] = {
        std::make_unique<HealthPackConfig>(),
        std::make_unique<WeaponAxeConfig>(),
        std::make_unique<WeaponBatConfig>(),
        std::make_unique<WeaponSwordConfig>()
    };

    for (auto& registrable : registrables) {
        registrable->register_prefab(scene);
    }
}

void SwampAutumScene::load(Scene& scene) {
    SwampAutumScene::load_camera();
    SwampAutumScene::load_map(std::string(Assets::MAP_SWAMP_AUTUM));

    Engine& engine = Engine::instance();
    auto& multiplayer_service = engine.services->get_service<MultiplayerService>().get();
    auto& prefab_service = engine.services->get_service<PrefabService>().get();
    RoundController& round_controller = add_multiplayer_round_controller();
    
    std::optional<std::reference_wrapper<GameObject>> multiplayer_controller_opt;

    for (auto& obj : scene.game_objects()) {
        if (obj.get().name() == "MultiplayerController") {
            multiplayer_controller_opt = obj;
            break;
        }
    }

    if (multiplayer_controller_opt.has_value()) {
        auto& multiplayer_controller = multiplayer_controller_opt.value().get();
        if (auto multiplayer_controller_behavior_opt = multiplayer_controller.get_script<BehaviorScript, MultiplayerController>(); multiplayer_controller_behavior_opt.has_value()) {
            auto& multiplayer_controller_behavior = multiplayer_controller_behavior_opt.value().get();

            for (auto& [key, value] : multiplayer_controller_behavior.users()) {
                auto& player = *dynamic_cast<PlayerObject*>(&prefab_service.instantiate("PlayerObject", scene, value.uuid).get());

                if (value.uuid == multiplayer_service.get_uuid()) {
                    player.set_local_player();
                    player.set_controllable();
                }

                round_controller.add_player(player);
            }
        }
    }

    if (multiplayer_service.get_peer_type() == PeerType::HOST) {
        register_host_handlers(multiplayer_service, scene, prefab_service);

        SwampAutumScene::load_interactables(scene);
    } else {
        register_client_handlers(multiplayer_service, scene, prefab_service, round_controller);
    }
}

void SwampAutumScene::register_host_handlers(MultiplayerService& multiplayer_service, Scene& scene, PrefabService& prefab_service) {
     multiplayer_service.register_handler(CustomMessageTypes::USER_MOVE, [this, &multiplayer_service, &scene](const Message& message) {
        MsgUserMove data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        Message msg = serialize_message(data, CustomMessageTypes::USER_MOVE);
        multiplayer_service.send(msg);

        handle_player_movement_update(data);
    });

    multiplayer_service.register_handler(CustomMessageTypes::USER_ATTACK, [this, &multiplayer_service, &scene](const Message& message) {
        MsgUserAttack data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        Message msg = serialize_message(data, CustomMessageTypes::USER_ATTACK);
        multiplayer_service.send(msg);

        handle_player_attack(data);
    });

    multiplayer_service.register_handler(CustomMessageTypes::USER_DROP_WEAPON, [this, &multiplayer_service, &scene](const Message& message) {
        MsgUserDropWeapon data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        Message msg = serialize_message(data, CustomMessageTypes::USER_DROP_WEAPON);
        multiplayer_service.send(msg);

        if (multiplayer_service.get_uuid() == data.uuid) return;

        handle_player_drop_weapon(data);
    });
}

void SwampAutumScene::register_client_handlers(MultiplayerService& multiplayer_service, Scene& scene, PrefabService& prefab_service, RoundController& controller) {
    multiplayer_service.register_handler(CustomMessageTypes::USER_MOVE, [this, &multiplayer_service, &scene](const Message& message) {
        MsgUserMove data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        if (multiplayer_service.get_uuid() == data.uuid) return;

        handle_player_movement_update(data);
    });

    multiplayer_service.register_handler(CustomMessageTypes::USER_ATTACK, [this, &multiplayer_service, &scene](const Message& message) {
        MsgUserAttack data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        if (multiplayer_service.get_uuid() == data.uuid) return;

        handle_player_attack(data);
    });

    multiplayer_service.register_handler(CustomMessageTypes::USER_DROP_WEAPON, [this, &multiplayer_service, &scene](const Message& message) {
        MsgUserDropWeapon data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        if (multiplayer_service.get_uuid() == data.uuid) return;

        handle_player_drop_weapon(data);
    });

    multiplayer_service.register_handler(CustomMessageTypes::USER_RESPAWN, [this, &controller](const Message& message) {
        MsgUserRespawn data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        if (auto player_opt = get_network_player_object(data.uuid); player_opt.has_value()) {
            controller.respawn_player(player_opt.value().get(), {data.x, data.y, data.z});
        }
    });

    multiplayer_service.register_handler(CustomMessageTypes::DROP_SPAWN, [this, &prefab_service, &scene](const Message& message) {
        MsgDropSpawn data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        auto& drop_obj = prefab_service.instantiate(data.drop_type, scene, data.drop_uuid).get();

        for (auto& obj : scene.game_objects()) {
            auto comp_opt = obj.get().get_component<NetworkIdentity>();
            if (!comp_opt.has_value()) continue;

            auto& comp = comp_opt.value().get();
            if (comp.uuid() != data.spawner_uuid) continue;

            for (auto& behavior : obj.get().get_components<BehaviorScript>()) {
                if (auto *dropper_comp = dynamic_cast<ItemDropper*>(&behavior.get().behavior())) {
                    dropper_comp->spawn_obj(drop_obj);
                }
            }
        }
    });

    multiplayer_service.register_handler(CustomMessageTypes::ROUND_END, [this, &controller, &scene](const Message& message) {
        MsgRoundEnd data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        for (auto& object_ref : scene.game_objects()) {
            auto& object = object_ref.get();
            
            auto round_controller_opt = object.get_script<BehaviorScript, RoundController>();
            if (round_controller_opt.has_value()) {
                round_controller_opt->get().round_end(!data.draw, data.winner_uuid);
                return;
            }
        }
    });
}