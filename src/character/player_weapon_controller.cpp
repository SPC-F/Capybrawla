#include <game/character/player_weapon_controller.h>

#include <game/behaviors/interactable/weapon_axe.h>
#include <game/behaviors/weapon_melee_behavior.h>
#include <game/character/player_object.h>
#include <game/network/message_types.h>
#include <game/prefabs/interactables/weapon_axe.h>
#include <game/prefabs/interactables/weapon_bat.h>
#include <game/prefabs/interactables/weapon_sword.h>
#include <game/prefabs/weapons/weapon_axe_player_object.h>
#include <game/prefabs/weapons/weapon_bat_player_object.h>
#include <game/prefabs/weapons/weapon_sword_player_object.h>

#include <engine/core/engine.h>
#include <engine/input/input_manager.h>
#include <engine/input/input_system.h>
#include <engine/network/multiplayer_service.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/components/rigidbody_2d.h>

constexpr float drop_weapon_offset_x = 32.0f;
constexpr float drop_weapon_offset_y = 0.0f;

PlayerWeaponController::PlayerWeaponController(GameObject& weapon)
    : Behavior(), default_weapon_(weapon), found_weapon_(std::nullopt) {}

void PlayerWeaponController::on_update(float dt) {
    const auto& provider =
        Engine::instance().services->get_service<InputManager>().get().provider();
    
    if (!PlayerObject::is_local(this->game_object())) return;

    if (provider.is_key_pressed(KeyCode::q)) {  
        drop_found_weapon();

        if (PlayerObject::is_multiplayer(this->game_object()))  {
            send_drop_weapon_message();
        }
    }
}

void PlayerWeaponController::drop_found_weapon() {
    if (!found_weapon_.has_value()) return;
    default_weapon_.get().set_active();

    auto player_sprite = game_object().get_component<Sprite>();
    if (!player_sprite.has_value()) throw std::runtime_error("PlayerWeaponController requires a Sprite component on the parent GameObject.");

    bool facing_right = player_sprite->get().flip_x() == false;

    float spawn_offset_x = facing_right ? drop_weapon_offset_x : -drop_weapon_offset_x;
    Vector3 spawn_pos = game_object().transform().position() + Vector3{spawn_offset_x, drop_weapon_offset_y, 0.0f};

    auto& scene = game_object().scene();
    if (dynamic_cast<WeaponAxePlayerObject*>(&found_weapon_->get()))          scene.add_game_object<WeaponAxePrefab>(scene, true, spawn_pos);
    else if (dynamic_cast<WeaponBatPlayerObject*>(&found_weapon_->get()))     scene.add_game_object<WeaponBatPrefab>(scene, true, spawn_pos);
    else if (dynamic_cast<WeaponSwordPlayerObject*>(&found_weapon_->get()))   scene.add_game_object<WeaponSwordPrefab>(scene, true, spawn_pos);

    auto& weapon = found_weapon_->get();
    found_weapon_ = std::nullopt;
    weapon.set_inactive();
    weapon.mark_for_deletion();
}

void PlayerWeaponController::send_drop_weapon_message() {
    MultiplayerService& multiplayer_service =
        Engine::instance().services->get_service<MultiplayerService>().get();

    MsgUserDropWeapon body{};
    std::strncpy(body.uuid, multiplayer_service.get_uuid().c_str(), sizeof(body.uuid) - 1);

    Message msg = serialize_message(body, CustomMessageTypes::USER_DROP_WEAPON);
    multiplayer_service.send(msg);
}

bool PlayerWeaponController::has_found_weapon() const {
    return found_weapon_.has_value();
}