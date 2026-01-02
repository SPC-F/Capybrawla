#include <game/prefabs/interactables/config/weapon_axe_config.h>
#include <game/prefabs/interactables/weapon_axe.h>

#include <engine/core/engine.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/gameObject.h>
#include <engine/public/prefab_service.h>

WeaponAxeConfig::WeaponAxeConfig(Scene& scene)
    : PrefabRegistrable("WeaponAxe", "WeaponAxe", 128.0f, 128.0f, 16.0f, 8.0f) {}

void WeaponAxeConfig::register_prefab(Scene& scene) {
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();

    if (prefab_service.has_prefab(prefab_name_)) return;

    prefab_service.register_prefab(prefab_name_, [this](Scene& scene, const std::string& name) -> GameObject& {
        auto& obj = scene.add_game_object<WeaponAxePrefab>(scene);
        
        obj.prefab_type_id(prefab_id_);
        obj.add_component<NetworkIdentity>(name.c_str());

        return obj;
    });
}