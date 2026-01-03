#pragma once

#include <game/assets.h>
#include <game/behaviors/multiplayer/multiplayer_controller.h>
#include <game/character/player_object.h>
#include <game/network/message_types.h>
#include <game/round/roundcontroller.h>
#include <game/scenes/level.h>

#include <engine/public/scene.h>
#include <engine/network/multiplayer_service.h>
#include <engine/public/prefab_service.h>

class SwampAutumScene : public Level {
public:
    SwampAutumScene();
    virtual ~SwampAutumScene() = default;

    virtual void setup(Scene& scene) override;
    virtual void load(Scene& scene) override;
    
private:
    float out_of_bounds_margin_x_ = 480.0f;
    float out_of_bounds_margin_y_ = 270.0f;
    float map_width_ = 1920.0f;
    float map_height_ = 1080.0f;
    ConnectionState connection_state_ = ConnectionState::NONE;

    void load_interactables(Scene& scene);
    PlayerObject& create_player_object(const std::string& name);
    GameObject& create_interactable_dropper(const std::string& name);
    RoundController& add_multiplayer_round_controller();
    MultiplayerController& add_multiplayer_controller();

    void handle_player_movement_update(const MsgUserMove& data);
    void handle_player_attack(const MsgUserAttack& data);
    void handle_player_drop_weapon(const MsgUserDropWeapon& data);

    void register_host_handlers(MultiplayerService& multiplayer_service, Scene& scene, PrefabService& prefab_service);
    void register_client_handlers(MultiplayerService& multiplayer_service, Scene& scene, PrefabService& prefab_service);

    std::optional<std::reference_wrapper<PlayerObject>> get_network_player_object(const std::string& uuid);
};