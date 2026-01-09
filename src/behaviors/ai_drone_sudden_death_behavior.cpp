#include <game/behaviors/ai_drone_sudden_death_behavior.h>
#include <engine/public/scene.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/core/engine.h>
#include <game/character/player_controller.h>
#include <limits>
#include <cmath>

void AIDroneSuddenDeathBehavior::on_awake() {
    ai_controller_ = this->get_component<AIController>();
    audio_service_ = Engine::instance().services->get_service<AudioService>();

    auto game_objects = this->game_object().scene().game_objects();
    for (auto& obj_ref : game_objects) {
        auto& obj = obj_ref.get();
        auto behavior_scripts = obj.get_components<BehaviorScript>();
        for (auto& script_ref : behavior_scripts) {
            auto& script = script_ref.get();
            try {
                auto& behavior = script.behavior();
                if (auto timer = dynamic_cast<RoundTimer*>(&behavior)) {
                    round_timer_ = std::ref(*timer);
                    break;
                }
            } catch (...) {}
        }
        if (round_timer_) break;
    }
}

void AIDroneSuddenDeathBehavior::on_update(float dt) {
    if (!ai_controller_) {
        ai_controller_ = this->get_component<AIController>();
    }

    if (!round_timer_) {
        auto game_objects = this->game_object().scene().game_objects();
        for (auto& obj_ref : game_objects) {
            auto& obj = obj_ref.get();
            auto behavior_scripts = obj.get_components<BehaviorScript>();
            for (auto& script_ref : behavior_scripts) {
                auto& script = script_ref.get();
                try {
                    auto& behavior = script.behavior();
                    if (auto timer = dynamic_cast<RoundTimer*>(&behavior)) {
                        round_timer_ = std::ref(*timer);
                        break;
                    }
                } catch (...) {}
            }
            if (round_timer_) break;
        }
    }

    if (!ai_controller_ || !round_timer_)
    {
        return;
    }

    if (round_timer_->get().remaining_time() <= 0) {
        auto game_objects = this->game_object().scene().game_objects();
        GameObject* closest_player = nullptr;
        float min_distance = std::numeric_limits<float>::max();

        Vector3 drone_pos = this->transform().position();

        for (auto& obj_ref : game_objects) {
            auto& obj = obj_ref.get();
            if (obj.tag() == "Player") {
                Vector3 diff = drone_pos - obj.transform().position();
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                if (distance < min_distance) {
                    min_distance = distance;
                    closest_player = &obj;
                }
            }
        }

        if (closest_player) {
            ai_controller_->get().set_chase_target(*closest_player);
            ai_controller_->get().set_mode(AIControllerMode::CHASE);

            // Attack logic
            if (min_distance <= ai_controller_->get().get_arrival_threshold()) {
                attack_cooldown_ -= dt;
                if (attack_cooldown_ <= 0) {
                    auto player_controller = closest_player->get_script<BehaviorScript, PlayerController>();
                    if (player_controller) {
                        if (audio_service_) {
                            audio_service_->get().play_sound("drone_shoot", 0.05f, false);
                        }
                        player_controller->get().hit(45);
                        attack_cooldown_ = attack_interval_;
                    }
                }
            }
        }
    }
}