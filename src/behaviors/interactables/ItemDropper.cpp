#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/prefab_service.h>
#include <engine/public/components/network_identity.h>
#include <engine/util/uuid.h>

#include <game/behaviors/interactable/ItemDropper.h>
#include <game/prefabs/interactables/health_pack.h>
#include <game/prefabs/weapons/weapon_axe_player_object.h>

GameObject& ItemDropper::random_drop() {
    const int random_index = std::rand() % drops_.size();
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();
    Scene& scene = Engine::instance().services->get_service<SceneService>().get().current_scene().value();

    latest_obj_ = uuid::generate_uuid_v4();

    return prefab_service.instantiate(drops_[random_index], scene, latest_obj_);
}

void ItemDropper::on_start() {
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();
    if (!prefab_service.has_prefab("HealthPack")) {
        prefab_service.register_prefab("HealthPack", [this](Scene& scene, const std::string& name) -> GameObject& {
            auto& obj = scene.add_game_object<HealthPackPrefab>(scene);
            obj.add_component<NetworkIdentity>(name.c_str());

            return obj;
        });
    }

    drops_.emplace_back("HealthPack");
}

void ItemDropper::on_update(float dt)
{
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