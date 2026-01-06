#pragma once

#include <game/character/player_object.h>
#include <game/scenes/level.h>

class LobbyScene : public Level {
public:
    LobbyScene();
    virtual ~LobbyScene() = default;

    virtual void setup(Scene& scene) override;
    virtual void load(Scene& scene) override;

private:
    UIButton& create_button(Scene& scene, const std::string& label, float position_x, float position_y, const std::string& texture_id);
    UIText& player_name(Scene &scene, const std::string& username, float frame_width, float font_size);
};