#include <game/prefabs/config/weapon_sword_config.h>
#include <game/prefabs/interactables/weapon_sword.h>

#include <engine/core/engine.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/gameObject.h>
#include <engine/public/prefab_service.h>

WeaponSwordConfig::WeaponSwordConfig()
    : PrefabRegistrable("WeaponSword", "WeaponSword", 32.0f, 32.0f, 16.0f, 8.0f) {}

void WeaponSwordConfig::register_prefab(Scene& scene) {
    auto& prefab_service = Engine::instance().services->get_service<PrefabService>().get();

    if (prefab_service.has_prefab(prefab_name_)) return;

    /// Capture prefab_id_ and prefab_name_ by value to use inside the lambda
    auto prefab_id = prefab_id_;
    auto prefab_name = prefab_name_;

    prefab_service.register_prefab(prefab_name, [prefab_id](Scene& scene, const std::string& name) -> GameObject& {
        auto& obj = scene.add_game_object<WeaponSwordPrefab>(scene);
        
        obj.prefab_type_id(prefab_id);
        obj.add_component<NetworkIdentity>(name.c_str());

        return obj;
    });
}