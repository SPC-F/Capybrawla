#include <game/prefabs/prefab_registrable.h>

PrefabRegistrable::PrefabRegistrable(
    std::string prefab_name, std::string prefab_id,
    float width, float height,
    float offset_x, float offset_y
) : prefab_name_(std::move(prefab_name)), prefab_id_(std::move(prefab_id)), width_(width), height_(height), offset_x_(offset_x), offset_y_(offset_y) {}

std::string PrefabRegistrable::prefab_name() const { return prefab_name_; }
std::string PrefabRegistrable::prefab_id() const { return prefab_id_; }

float PrefabRegistrable::width() const { return width_; }
float PrefabRegistrable::height() const { return height_; }

float PrefabRegistrable::offset_x() const { return offset_x_; }
float PrefabRegistrable::offset_y() const { return offset_y_; }