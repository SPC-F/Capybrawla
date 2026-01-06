#include <game/behaviors/ui/end_round_continue_behavior.h>

#include <engine/core/engine.h>
#include <engine/public/components/sprite.h>
#include <engine/public/scene_service.h>

EndRoundContinueBehavior::EndRoundContinueBehavior(float disappear_time)
    : time_before_disappear_(disappear_time) {}

void EndRoundContinueBehavior::on_update(float dt) {
    accumulated_time_ += dt;

    if (accumulated_time_ >= time_before_disappear_) {
        game_object().mark_for_deletion();

        auto& scene_service = Engine::instance().services->get_service<SceneService>().get();
        scene_service.load_scene("Level_LobbyScene");
    }
}