#include <game/behaviors/interactable/ItemDropper.h>

#include <iostream>

#include "engine/core/engine.h"
#include "engine/public/scene_service.h"
#include "engine/public/components/rigidbody_2d.h"
#include "game/prefabs/interactables/health_pack.h"
#include "game/prefabs/weapons/weapon_axe_player_object.h"

// std::unique_ptr<GameObject> ItemDropper::random_drop() const
// {
//     const int random_index = std::rand() % drops_.size();
//     return drops_[random_index]->clone();
// }

void ItemDropper::on_start()
{
    Scene& scene = Engine::instance().services->get_service<SceneService>().get().current_scene().value();
    drops_.emplace_back(std::make_unique<HealthPackPrefab>(scene));
}

void ItemDropper::on_update(float dt)
{
    remaining_time_before_drop -= dt;
    if(remaining_time_before_drop <= 0)
    {
        // auto drop = random_drop();
        const auto position = game_object().transform().position();
        const auto half_position = position - Vector3{16, 16, 0};
        // drop->get_component<Rigidbody2D>()->get().teleport(half_position);

        Scene& scene = Engine::instance().services->get_service<SceneService>().get().current_scene().value();
        // scene.add_game_object(std::move(drop));

        remaining_time_before_drop = default_drop_duration_secs;
    }
}