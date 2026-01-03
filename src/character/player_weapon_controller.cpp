#include <game/character/player_weapon_controller.h>

#include <game/behaviors/weapon_melee_behavior.h>
#include <game/character/player_object.h>
#include <game/network/message_types.h>

#include <engine/core/engine.h>
#include <engine/input/input_manager.h>
#include <engine/input/input_system.h>
#include <engine/network/multiplayer_service.h>
#include <engine/public/components/network_identity.h>

PlayerWeaponController::PlayerWeaponController(GameObject& weapon)
    : Behavior(), default_weapon_(weapon), found_weapon_(std::nullopt) {}

void PlayerWeaponController::on_start() {
    auto& children = game_object().children();

    for (auto& child_ref : children) {
        auto& child = child_ref.get();
        if (!child.get_component<BehaviorScript>()) continue;

        auto& behavior = child.get_component<BehaviorScript>()->get().behavior();
        if (!dynamic_cast<WeaponMeleeBehavior*>(&behavior)) continue;

        if (child.id() != default_weapon_.get().id() && child.is_active()) {
            switch_weapon(child);
            break;
        }
    }
}

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
    destroy_found_weapon();
}

void PlayerWeaponController::destroy_found_weapon() {
    if (!found_weapon_.has_value()) return;

    default_weapon_.get().set_active();

    auto& weapon = found_weapon_->get();
    found_weapon_ = std::nullopt;
    weapon.set_inactive();
    weapon.mark_for_deletion();
}

void PlayerWeaponController::switch_weapon(std::optional<std::reference_wrapper<GameObject>> new_weapon) {    
    if (!new_weapon.has_value()) return;

    drop_found_weapon();

    auto& weapon = new_weapon->get();
    found_weapon_ = new_weapon;
    weapon.set_active();
    
    default_weapon_.get().set_inactive();
}

void PlayerWeaponController::send_drop_weapon_message() {
    MultiplayerService& multiplayer_service =
        Engine::instance().services->get_service<MultiplayerService>().get();

    MsgUserDropWeapon body{};
    std::strncpy(body.uuid, multiplayer_service.get_uuid().c_str(), sizeof(body.uuid) - 1);

    Message msg = serialize_message(body, CustomMessageTypes::USER_DROP_WEAPON);
    multiplayer_service.send(msg);
}