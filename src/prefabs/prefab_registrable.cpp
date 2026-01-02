#include <game/prefabs/prefab_registrable.h>

PrefabRegistrable::PrefabRegistrable(std::string prefab_name, std::string prefab_id)
    : prefab_name_(std::move(prefab_name)), prefab_id_(std::move(prefab_id)) {}

std::string PrefabRegistrable::prefab_name() const {
    return prefab_name_;
}

std::string PrefabRegistrable::prefab_id() const {
    return prefab_id_;
}