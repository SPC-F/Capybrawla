#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/prefab_service.h>
#include <engine/public/components/network_identity.h>
#include <engine/util/uuid.h>
#include <engine/network/multiplayer_service.h>

#include <game/behaviors/interactable/ItemDropper.h>
#include <game/prefabs/interactables/health_pack.h>
#include <game/prefabs/weapons/weapon_axe_player_object.h>
#include <game/network/message_types.h>

GameObject& ItemDropper::random_drop() {
    const int random_index = std::rand() % drops_.size();
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();
    Scene& scene = Engine::instance().services->get_service<SceneService>().get().current_scene().value();

    latest_obj_ = uuid::generate_uuid_v4();
    std::string drop = drops_[random_index];

    auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();
    if (multiplayer_service.get_peer_type() == PeerType::HOST) {
        auto network_id_opt = game_object().get_component<NetworkIdentity>();
        if (network_id_opt.has_value()) {
            MsgDropSpawn body{};
            std::strncpy(body.spawner_uuid, network_id_opt.value().get().uuid().c_str(), sizeof(body.spawner_uuid) - 1);
            std::strncpy(body.drop_uuid, latest_obj_.c_str(), sizeof(body.drop_uuid) - 1);
            std::strncpy(body.drop_type, drop.c_str(), sizeof(body.drop_type) - 1);

            Message msg = serialize_message(body, CustomMessageTypes::DROP_SPAWN);
            multiplayer_service.send(msg);
        }
    }

    return prefab_service.instantiate(drop, scene, latest_obj_);
}

void ItemDropper::on_start() {
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();
    if (!prefab_service.has_prefab("HealthPack")) {
        prefab_service.register_prefab("HealthPack", [this](Scene& scene, const std::string& name) -> GameObject& {
            auto& obj = scene.add_game_object<HealthPackPrefab>(scene);
            obj.add_component<NetworkIdentity>(name.c_str());
            obj.prefab_type_id("HealthPack");

            return obj;
        });
    }

    drops_.emplace_back("HealthPack");
}

void ItemDropper::on_update(float dt)
{
    auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();
    if (multiplayer_service.get_peer_type() == PeerType::CLIENT) return;

    bool latest_spawn_used = true;
    for (auto& obj : game_object().scene().game_objects()) {
        auto comp = obj.get().get_component<NetworkIdentity>();
        if (comp.has_value() && latest_obj_ == comp.value().get().uuid()) {
            latest_spawn_used = false;
            break;
        }
    }

    if (latest_spawn_used) {
        remaining_time_before_drop -= dt;
        if(remaining_time_before_drop <= 0) {
            auto& drop = random_drop();
            const auto position = game_object().transform().position();
            const auto half_position = position - Vector3{16, 16, 0};
            drop.get_component<Rigidbody2D>()->get().teleport(half_position);

            remaining_time_before_drop = default_drop_duration_secs;
        }
    }
}

void ItemDropper::spawn_obj(GameObject& obj) {
    auto& comp = obj.get_component<NetworkIdentity>().value().get();
    latest_obj_ = comp.uuid();

    const auto position = game_object().transform().position();
    const auto half_position = position - Vector3{16, 16, 0};
    obj.get_component<Rigidbody2D>()->get().teleport(half_position);
}