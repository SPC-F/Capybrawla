#pragma once

#include <engine/public/scene.h>

class PrefabRegistrable
{
public:
    explicit PrefabRegistrable(
        std::string prefab_name, std::string prefab_id, 
        float width = 0.0f, float height = 0.0f, 
        float offset_x = 0.0f, float offset_y = 0.0f
    );

    virtual ~PrefabRegistrable() = default;
    virtual void register_prefab(Scene& scene) = 0;

    std::string prefab_name() const;
    std::string prefab_id() const;

    float width() const;
    float height() const;

    float offset_x() const;
    float offset_y() const;
protected:
    std::string prefab_name_;
    std::string prefab_id_;

    float width_;
    float height_;

    float offset_x_ = 0.0f;;
    float offset_y_ = 0.0f;
};