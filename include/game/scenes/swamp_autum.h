#pragma once

#include <game/assets.h>

#include <engine/public/scene.h>
#include <engine/network/multiplayer_service.h>
#include <game/behaviors/multiplayer/multiplayer_controller.h>
#include <game/character/player_object.h>
#include <game/round/roundcontroller.h>
#include <game/scenes/level.h>

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

    PlayerObject& create_player_object(Scene& scene, const std::string& name);
    RoundController& add_multiplayer_round_controller(Scene& scene);
    MultiplayerController& add_multiplayer_controller(Scene& scene);
};