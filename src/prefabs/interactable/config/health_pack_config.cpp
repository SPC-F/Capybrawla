#include <game/prefabs/interactables/config/health_pack_config.h>
#include <game/prefabs/interactables/health_pack.h>

#include <engine/core/engine.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/gameObject.h>
#include <engine/public/prefab_service.h>

HealthPackConfig::HealthPackConfig(Scene& scene)
    : PrefabRegistrable("HealthPack", "HealthPack") {}

void HealthPackConfig::register_prefab(Scene& scene) {
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();

    if (prefab_service.has_prefab(prefab_name_)) return;

    prefab_service.register_prefab(prefab_name_, [this](Scene& scene, const std::string& name) -> GameObject& {
        auto& obj = scene.add_game_object<HealthPackPrefab>(scene);
        
        obj.prefab_type_id(prefab_id_);
        obj.add_component<NetworkIdentity>(name.c_str());

        return obj;
    });
}