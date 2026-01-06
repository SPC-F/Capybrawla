#pragma once

#include <game/character/player_object.h>
#include <game/scenes/level.h>
#include <game/behaviors/multiplayer/multiplayer_controller.h>

class LobbyScene : public Level {
public:
    LobbyScene();
    virtual ~LobbyScene() = default;

    virtual void setup(Scene& scene) override;
    virtual void load(Scene& scene) override;

private:
    GameObject& multiplayer_controller_;
    std::vector<GameObject*> player_frames_;

    GameObject& create_multiplayer_controller();

    void register_host_handlers(MultiplayerService& multiplayer_service);
    void register_client_handlers(MultiplayerService& multiplayer_service);

    void update_player_displays(MultiplayerService& multiplayer_service);

    void create_ip_text();
    void create_player_displays(MultiplayerService& multiplayer_service);
    void create_start_button();

    UIButton& create_button(Scene& scene, const std::string& label, float position_x, float position_y, const std::string& texture_id);
    UIText& player_name(Scene &scene, const std::string& username, float frame_width, float font_size);
};