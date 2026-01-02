#include <game/behaviors/interactable/ItemDropper.h>
#include <game/prefabs/interactables/health_pack.h>
#include <game/prefabs/weapons/weapon_axe_player_object.h>
#include <game/network/message_types.h>

#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/prefab_service.h>
#include <engine/public/components/network_identity.h>
#include <engine/network/multiplayer_service.h>
#include <engine/util/uuid.h>


ItemDropper::ItemDropper(std::vector<std::unique_ptr<PrefabRegistrable>> registrable_drops) 
    : registrable_drops_(std::move(registrable_drops)) {}

GameObject& ItemDropper::random_drop() {
    const int random_index = std::rand() % drops_.size();
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();
    Scene& scene = Engine::instance().services->get_service<SceneService>().get().current_scene().value();

    current_drop_ = drops_[random_index];
    latest_obj_ = uuid::generate_uuid_v4();
    std::string drop = drops_[random_index];

    auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();
    if (multiplayer_service.get_peer_type() == PeerType::HOST) {
        MsgDropSpawn body{};
        std::strncpy(body.spawner_uuid, game_object().get_component<NetworkIdentity>().value().get().uuid().c_str(), sizeof(body.spawner_uuid) - 1);
        std::strncpy(body.drop_uuid, latest_obj_.c_str(), sizeof(body.drop_uuid) - 1);
        std::strncpy(body.drop_type, drop.c_str(), sizeof(body.drop_type) - 1);

        Message msg = serialize_message(body, CustomMessageTypes::DROP_SPAWN);
        multiplayer_service.send(msg);
    }

    return prefab_service.instantiate(drop, scene, latest_obj_);
}

void ItemDropper::on_start() {
    for (auto& drop : registrable_drops_) {
        drop->register_prefab(game_object().scene());
        drops_.emplace_back(drop->prefab_name());
    }
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
            
            auto& current_registrable = get_registrable_drop(current_drop_);

            // const auto half_position = position - Vector3{16, 16, 0};
            const auto half_position = position + Vector3{current_registrable.get()->offset_x(), current_registrable.get()->offset_y(), 0};
            drop.get_component<Rigidbody2D>()->get().teleport(half_position);

            remaining_time_before_drop = default_drop_duration_secs;
        }
    }
}

void ItemDropper::spawn_obj(GameObject& obj) {
    auto& comp = obj.get_component<NetworkIdentity>().value().get();
    latest_obj_ = comp.uuid();

    const auto position = game_object().transform().position();

    auto& current_registrable = get_registrable_drop(current_drop_);


    // const auto half_position = position - Vector3{16, 16, 0};
    const auto half_position = position + Vector3{current_registrable.get()->offset_x(), current_registrable.get()->offset_y(), 0};
    obj.get_component<Rigidbody2D>()->get().teleport(half_position);
}

std::unique_ptr<PrefabRegistrable>& ItemDropper::get_registrable_drop(const std::string& prefab_name) {
    for (auto& registrable : registrable_drops_) {
        if (registrable->prefab_name() == prefab_name) {
            return registrable;
        }
    }

    throw std::runtime_error("No registrable found for drop: " + prefab_name);
}