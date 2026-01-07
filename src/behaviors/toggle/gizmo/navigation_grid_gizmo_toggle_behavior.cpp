#include <game/behaviors/toggle/gizmo/navigation_grid_gizmo_toggle_behavior.h>

#include <engine/core/engine.h>
#include <engine/input/input_system.h>
#include <engine/input/input_manager.h>
#include <engine/public/components/ai/navigation/navigation_graph.h>
#include <engine/public/components/ai/navigation/navigation_node.h>
#include <engine/public/components/ai/navigation/pathfinding.h>
#include <engine/public/gameObject.h>
#include <engine/public/scene.h>

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

void NavigationGridGizmoToggleBehavior::on_update(float dt) {
    
    const IInputProvider &provider =
    Engine::instance().services->get_service<InputManager>().get().provider();
    
    if (!provider.is_key_pressed(KeyCode::f3)) return; 

    std::optional<std::reference_wrapper<GameObject>> tilemap_obj_opt = std::nullopt;
    
    for (auto& obj : game_object().scene().game_objects()) {
        if (obj.get().name() == "TileMapDynamic_Parent") {
            tilemap_obj_opt = obj;
            break;
        }
    }

    if (!tilemap_obj_opt.has_value()) 
        throw std::runtime_error("AIRenderingBehavior could not find TileMapDynamic_Parent in the scene.");

    auto& tilemap_obj = tilemap_obj_opt->get();
    auto maybe_nav_graph = tilemap_obj.get_component<NavigationGraph>();

    if (!maybe_nav_graph) {
        throw std::runtime_error("AIRenderingBehavior requires a NavigationGraph component on the tilemap parent.");
    }

    auto& nav_graph = maybe_nav_graph->get();
    toggle_navigation_graph(tilemap_obj);
}