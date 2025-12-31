#pragma once

#include <engine/public/scene.h>

#include <functional>

class Level {
public:
    Level(const std::string& name, bool reload = true);
    virtual ~Level() = default;

    std::string name() const;

    void init();
    void unload(Scene& scene);

    virtual void setup(Scene& scene) {};
    virtual void load(Scene& scene) {};

    void load_camera(int window_width = 1920, int window_height = 1080);
    void load_map(const std::string& map_name);

    size_t add_on_start_callback(std::function<void(Scene&)> callback) {
        on_start_callbacks.push_back(callback);
        return on_start_callbacks.size() - 1;
    }

    size_t add_on_stop_callback(std::function<void(Scene&)> callback) {
        on_stop_callbacks.push_back(callback);
        return on_stop_callbacks.size() - 1;
    }

    bool remove_on_start_callback(size_t index) {
        if (index >= on_start_callbacks.size()) {
            return false;
        }

        on_start_callbacks.erase(on_start_callbacks.begin() + index);
        return true;
    }

    bool remove_on_stop_callback(size_t index) {
        if (index >= on_stop_callbacks.size()) {
            return false;
        }

        on_stop_callbacks.erase(on_stop_callbacks.begin() + index);
        return true;
    }

    Scene& scene() {
        return scene_;
    }

private:
    Scene& scene_;
    std::string name_;
    bool reload_;

    std::vector<std::function<void(Scene&)>> on_start_callbacks;
    std::vector<std::function<void(Scene&)>> on_stop_callbacks;

    Scene& create_scene(const std::string& name);
};