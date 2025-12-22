#include <game/scenes/level.h>

#include <game/scenes/level_loader.h>

#include <engine/core/engine.h>
#include <engine/public/scene_service.h>
#include <engine/core/rendering/renderingService.h>

Scene& Level::create_scene(const std::string& name) {
    SceneService& scene_service = Engine::instance().services->get_service<SceneService>().get();
    return scene_service.add_scene(name);
}

Level::Level(const std::string& name, bool reload) : scene_(create_scene(name)), name_(name), reload_(reload) {}

std::string Level::name() const {
    return name_;
}

void Level::init() {
    setup(scene_);
    
    /// If reloading, then call load on run
    scene_.on_run([this](Scene& scene) {
        if (reload_) load(scene);

        for (const auto& callback : on_start_callbacks) {
            callback(scene);
        }
    });

    /// If reloading, unload on stop
    scene_.on_stop([this](Scene& scene) {
        for (const auto& callback : on_stop_callbacks) {
            callback(scene);
        }

        if (reload_) unload(scene);
    });

    /// If not reloading, load once during init
    if (!reload_) load(scene_);
}

void Level::unload(Scene& scene) {
    for (auto& obj_ref : scene.game_objects()) {
        auto& obj = obj_ref.get();

        if (!obj.dont_destroy_on_load()) {
            obj.mark_for_deletion();
        }
    }
    
    scene.cleanup_destroyed_game_objects();
}

void Level::load_camera(int window_width, int window_height) {
    RenderingService& rendering_service = Engine::instance().services->get_service<RenderingService>().get();
    int width = rendering_service.window().get_window_width();
    int height = rendering_service.window().get_window_height();

    if (width != window_width || height != window_height) {
        width = window_width;
        height = window_height;
    }

    auto& camera = scene_.add_game_object<Camera>(scene_, Color(), 1.0f, true);
    camera.transform().position({
        static_cast<float>(width) / 2.0f,
        static_cast<float>(height) / 2.0f, 0.0f});
}

void Level::load_map(const std::string& map_name) {
    LevelLoader loader;
    loader.load_game_objects_from_json(
        map_name,
        scene_
    );
}