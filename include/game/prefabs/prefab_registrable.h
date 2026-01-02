#pragma once

#include <engine/public/scene.h>

class PrefabRegistrable
{
public:
    explicit PrefabRegistrable(std::string prefab_name, std::string prefab_id);

    virtual ~PrefabRegistrable() = default;
    virtual void register_prefab(Scene& scene) = 0;

    std::string prefab_name() const;
    std::string prefab_id() const;
protected:
    std::string prefab_name_;
    std::string prefab_id_;
};