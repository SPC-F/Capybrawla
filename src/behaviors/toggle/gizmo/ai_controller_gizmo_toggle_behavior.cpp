#include <game/behaviors/toggle/gizmo/ai_controller_gizmo_toggle_behavior.h>

#include <engine/core/engine.h>
#include <engine/input/input_system.h>
#include <engine/input/input_manager.h>
#include <engine/public/components/ai/navigation/navigation_graph.h>
#include <engine/public/components/ai/navigation/navigation_node.h>
#include <engine/public/components/ai/ai_controller.h>
#include <engine/public/gameObject.h>
#include <engine/public/scene.h>

AIControllerGizmoToggleBehavior::AIControllerGizmoToggleBehavior(std::reference_wrapper<GameObject> ai_agent_object)
    : ai_agent_object_(ai_agent_object) {}

void AIControllerGizmoToggleBehavior::on_update(float dt) {
    
    const IInputProvider &provider =
    Engine::instance().services->get_service<InputManager>().get().provider();
    
    if (!provider.is_key_pressed(KeyCode::f4)) return; 

   
    auto maybe_ai_controller = ai_agent_object_.get().get_component<AIController>();

    if (!maybe_ai_controller) {
        throw std::runtime_error("AIControllerGizmoToggleBehavior requires an AIController component on the AI agent object.");
    }

    auto& ai_controller = maybe_ai_controller->get();
    
    if (ai_controller.should_draw())    ai_controller.disable_draw();
    else                                ai_controller.enable_draw();

}