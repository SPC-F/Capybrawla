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
#include <game/network/message_types.h>

#include <iostream>

const Vector3 DEFAULT_MULTIPLAYER_RESPAWN_POSITION = {600, 0, 0};

SwampAutumScene::SwampAutumScene() : Level("Level_SwampAutumScene") {}

PlayerObject& SwampAutumScene::create_player_object(Scene& scene, const std::string& name) {
    float start_x = 1000.0f;
    float start_y = 500.0f;

    auto& obj = scene.add_game_object<PlayerObject>(scene, Vector3{start_x, start_y, 0});
    obj.prefab_type_id("PlayerObject");
    obj.add_component<BehaviorScript>(std::make_unique<PlayerOutOfBoundsBehavior>(
      -out_of_bounds_margin_x_,
      map_width_ + out_of_bounds_margin_x_,
      -out_of_bounds_margin_y_,
      map_height_ + out_of_bounds_margin_y_));

    Engine& engine = Engine::instance();
    auto& multiplayer_service = engine.services->get_service<MultiplayerService>().get();
    obj.add_component<NetworkIdentity>(name.c_str());

    auto& weapon_axe = scene.add_game_object<WeaponAxePlayerObject>(scene, obj);

    obj.layer(Layers::Foreground);

    return obj;
}

RoundController& SwampAutumScene::add_multiplayer_round_controller(Scene& scene) {
    GameObject& wrapper = scene.add_game_object("RoundControllerWrapper");
    auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<RoundController>(DEFAULT_MULTIPLAYER_RESPAWN_POSITION));
    return *dynamic_cast<RoundController*>(&comp.behavior());
}

MultiplayerController& SwampAutumScene::add_multiplayer_controller(Scene& scene) {
    GameObject& wrapper = scene.add_game_object("MultiplayerControllerWrapper");
    auto& comp = wrapper.add_component<BehaviorScript>(std::make_unique<MultiplayerController>());
    return *dynamic_cast<MultiplayerController*>(&comp.behavior());
}

void SwampAutumScene::setup(Scene& scene) {
    add_on_stop_callback([](Scene& scene) {
        auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();
        multiplayer_service.disconnect();
    });

    PrefabService& prefab_service = Engine::instance().services->get_service<PrefabService>().get();
    prefab_service.register_prefab("PlayerObject", [this](Scene& scene, const std::string& name) -> GameObject& {
        return create_player_object(scene, name);
    });
}

void SwampAutumScene::load(Scene& scene) {
    SwampAutumScene::load_camera();
    SwampAutumScene::load_map(std::string(Assets::MAP_SWAMP_AUTUM));

    Engine& engine = Engine::instance();
    auto& multiplayer_service = engine.services->get_service<MultiplayerService>().get();
    auto& prefab_service = engine.services->get_service<PrefabService>().get();
    RoundController& controller = add_multiplayer_round_controller(scene);

    MultiplayerController& multiplayer_controller = add_multiplayer_controller(scene);
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

        auto& player = *dynamic_cast<PlayerObject*>(&prefab_service.instantiate("PlayerObject", scene, multiplayer_service.get_uuid().c_str()).get());
        player.set_controllable();
        controller.add_player(player);
    } else {
        multiplayer_controller.on_connection_state_change([&scene, &multiplayer_service](ConnectionState old_state, ConnectionState new_state) {
            if (new_state == ConnectionState::CONNECTED) {
                std::cout << "Connected with UUID " << multiplayer_service.get_uuid() << std::endl;

                MsgUserJoin data{};
                std::strncpy(data.uuid, multiplayer_service.get_uuid().c_str(), sizeof(data.uuid) - 1);

                Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
                multiplayer_service.send(msg);
            }
        });

        multiplayer_service.register_handler(CustomMessageTypes::USER_JOIN, [&multiplayer_service, &controller, &prefab_service, &scene](const Message& message) {
            MsgUserJoin data{};
            std::memcpy(&data, message.payload.data(), sizeof(data));

            std::cout << "New user joined with UUID " << data.uuid << std::endl;

            auto& player = *dynamic_cast<PlayerObject*>(&prefab_service.instantiate("PlayerObject", scene, data.uuid).get());
            controller.add_player(player);
            if (data.uuid == multiplayer_service.get_uuid())
                player.set_controllable();
        });
    }
}