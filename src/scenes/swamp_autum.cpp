#include <game/scenes/swamp_autum.h>
#include <engine/public/components/behaviorscript.h>

#include <game/scenes/level_loader.h>
#include <engine/public/util/layers.h>

#include <game/character/player_outofbounds_behavior.h>
#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/public/prefab_service.h>
#include <engine/network/multiplayer_service.h>
#include <engine/public/scene_service.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/sprite.h>
#include "engine/public/components/network_identity.h"
#include <game/round/roundcontroller.h>
#include <game/prefabs/weapons/weapon_bat_player_object.h>
#include <game/prefabs/weapons/weapon_axe_player_object.h>
#include <game/prefabs/weapons/weapon_sword_player_object.h>
#include <game/behaviors/multiplayer/multiplayer_controller.h>
#include <game/behaviors/weapon_melee_behavior.h>

#include <iostream>

SwampAutumScene::SwampAutumScene() : Level("Level_SwampAutumScene") {}

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

    auto& weapon_axe = scene().add_game_object<WeaponAxePlayerObject>(scene(), obj);

    obj.layer(Layers::Foreground);

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
    return *dynamic_cast<RoundController*>(&comp.behavior());
}

MultiplayerController& SwampAutumScene::add_multiplayer_controller() {
    GameObject& wrapper = scene().add_game_object("MultiplayerControllerWrapper");
    auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<MultiplayerController>());
    return *dynamic_cast<MultiplayerController*>(&comp.behavior());
}

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
        auto behaviors = child.get().get_components<BehaviorScript>();
        for (auto& behavior_ref : behaviors) {
            auto& behavior = behavior_ref.get().behavior();
            if (auto weapon_behavior = dynamic_cast<WeaponMeleeBehavior*>(&behavior); weapon_behavior != nullptr) {
                weapon_behavior->attack();
            }
        }
    }
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
}

void SwampAutumScene::load(Scene& scene) {
    SwampAutumScene::load_camera();
    SwampAutumScene::load_map(std::string(Assets::MAP_SWAMP_AUTUM));

    Engine& engine = Engine::instance();
    auto& multiplayer_service = engine.services->get_service<MultiplayerService>().get();
    auto& prefab_service = engine.services->get_service<PrefabService>().get();
    RoundController& controller = add_multiplayer_round_controller();

    MultiplayerController& multiplayer_controller = add_multiplayer_controller();
    if (multiplayer_service.get_peer_type() == PeerType::HOST) {
        multiplayer_service.set_max_clients(4);
        multiplayer_service.set_connection_port(1024);
        multiplayer_service.start_server();

        multiplayer_service.register_handler(CustomMessageTypes::USER_JOIN, [&multiplayer_service, &controller, &prefab_service, &scene](const Message& message) {
            MsgUserJoin data{};
            std::memcpy(&data, message.payload.data(), sizeof(data));

            std::cout << "New user joined with UUID " << data.uuid << std::endl;
            Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
            multiplayer_service.send(msg);

            auto& player = *dynamic_cast<PlayerObject*>(&prefab_service.instantiate("PlayerObject", scene, data.uuid).get());
            controller.add_player(player);
        });

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

        auto& player = *dynamic_cast<PlayerObject*>(&prefab_service.instantiate("PlayerObject", scene, multiplayer_service.get_uuid().c_str()).get());
        player.set_local_player();
        player.set_controllable();
        controller.add_player(player);
    } else {
        multiplayer_controller.on_connection_state_change([&scene, &multiplayer_service, &controller](ConnectionState old_state, ConnectionState new_state) {
            if (new_state == ConnectionState::CONNECTED) {
                std::cout << "Connected with UUID " << multiplayer_service.get_uuid() << std::endl;

                MsgUserJoin data{};
                std::strncpy(data.uuid, multiplayer_service.get_uuid().c_str(), sizeof(data.uuid) - 1);

                Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
                multiplayer_service.send(msg);

                // Sync unregistered player objects
                for (auto& game_object_ref : scene.game_objects()) {
                    auto& game_object = game_object_ref.get();
                    if (auto player = dynamic_cast<PlayerObject*>(&game_object)) {
                        if (!controller.is_player_registered(*player)) {
                            controller.add_player(*player);
                        }
                    }
                }
            }
        });

        multiplayer_service.register_handler(CustomMessageTypes::USER_JOIN, [&multiplayer_service, &controller, &prefab_service, &scene](const Message& message) {
            MsgUserJoin data{};
            std::memcpy(&data, message.payload.data(), sizeof(data));

            std::cout << "New user joined with UUID " << data.uuid << std::endl;

            auto& player = *dynamic_cast<PlayerObject*>(&prefab_service.instantiate("PlayerObject", scene, data.uuid).get());
            controller.add_player(player);
            if (data.uuid == multiplayer_service.get_uuid()) {
                player.set_local_player();
                player.set_controllable();
            }
        });

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
    }
}