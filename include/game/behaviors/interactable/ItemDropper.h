#pragma once

#include <game/prefabs/prefab_registrable.h>

#include <engine/public/behavior.h>

#include <string>

class ItemDropper : public Behavior {
private:
    std::vector<std::unique_ptr<PrefabRegistrable>> registrable_drops_;
    std::vector<std::string> drops_;
    std::string current_drop_;
    std::string latest_obj_;

    constexpr static double default_drop_duration_secs = 15.0;
    double remaining_time_before_drop = 0;

    std::unique_ptr<PrefabRegistrable>& get_registrable_drop(const std::string& prefab_name);

public:
    ItemDropper(std::vector<std::unique_ptr<PrefabRegistrable>> registrable_drops);

    [[nodiscard]] GameObject& random_drop();
    void spawn_obj(GameObject& obj);
    
    void on_start() override;
    void on_update(float dt) override;
};