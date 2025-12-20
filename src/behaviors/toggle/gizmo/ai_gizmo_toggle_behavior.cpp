#include <game/behaviors/toggle/gizmo/ai_gizmo_toggle_behavior.h>

#include <engine/core/engine.h>
#include <engine/input/input_system.h>
#include <engine/input/input_manager.h>
#include <engine/public/gameObject.h>
#include <engine/public/components/ai/navigation/navigation_graph.h>
#include <engine/public/components/ai/navigation/navigation_node.h>
#include <engine/public/components/ai/navigation/pathfinding.h>

AIGizmoToggleBehavior::AIGizmoToggleBehavior(std::reference_wrapper<GameObject> tilemap_parent)
    : tilemap_parent_(tilemap_parent) {}

void toggle_pathfinding(Pathfinding& pathfinding) {
    bool path_should_draw = !pathfinding.should_draw();

    if (path_should_draw != pathfinding.should_draw()) {
        if (path_should_draw) pathfinding.enable_draw();
        else pathfinding.disable_draw();
    }
}

void toggle_navigation_graph(GameObject& tilemap_obj) {
    for (auto& child : tilemap_obj.children()) {
        auto& child_obj = child.get();

        for (auto& child_nav_node_obj : child_obj.children()) {
            auto& nav_obj = child_nav_node_obj.get();
            auto maybe_nav_node = nav_obj.get_component<NavigationNode>();
                    
            if (!maybe_nav_node) continue;

            auto& nav_node = maybe_nav_node->get();
            
            bool should_draw = !nav_node.should_draw();

            if (should_draw != nav_node.should_draw()) {
                if (should_draw) nav_node.enable_draw();
                else nav_node.disable_draw();
            }
        }
        
    }
}

void AIGizmoToggleBehavior::on_update(float dt) {
    
    const IInputProvider &provider =
    Engine::instance().services->get_service<InputManager>().get().provider();
    
    if (!provider.is_key_pressed(KeyCode::f3) && !provider.is_key_pressed(KeyCode::f4)) return; 
    auto& tilemap_obj = tilemap_parent_.get();

    auto maybe_nav_graph = tilemap_obj.get_component<NavigationGraph>();

    if (!maybe_nav_graph) {
        throw std::runtime_error("AIRenderingBehavior requires a NavigationGraph component on the tilemap parent.");
    }

    auto maybe_pathfinding = tilemap_obj.get_component<Pathfinding>();
    
    if (!maybe_pathfinding) {
        throw std::runtime_error("AIRenderingBehavior requires a Pathfinding component on the owner GameObject.");
    }

    auto& nav_graph = maybe_nav_graph->get();
    auto& pathfinding = maybe_pathfinding->get();

    if (provider.is_key_pressed(KeyCode::f3)) toggle_navigation_graph(tilemap_obj);
    if (provider.is_key_pressed(KeyCode::f4)) toggle_pathfinding(pathfinding);
}