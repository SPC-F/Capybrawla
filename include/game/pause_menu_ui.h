#pragma once

#include "engine/public/components/ui/image.h"
#include "engine/public/components/ui/text.h"
#include "engine/public/ui/ui_object.h"
#include "engine/public/ui/interactable/ui_button.h"
#include "engine/public/util/layers.h"

constexpr float BUTTON_WIDTH = 400.0f;
constexpr float BUTTON_HEIGHT = 80.0f;
constexpr float BUTTON_FONT_SIZE = 46.0f;

constexpr int SCREEN_WIDTH = 1920.0f;
constexpr int SCREEN_HEIGHT = 1080.0f;

constexpr int CENTER_X = SCREEN_WIDTH / 2;
constexpr int CENTER_Y = SCREEN_HEIGHT / 2;

constexpr int BUTTON_SPACING = 20;
constexpr float BUTTON_START_Y = CENTER_Y - ((BUTTON_HEIGHT + BUTTON_SPACING) * 2);
constexpr float BUTTON_START_X = CENTER_X - (BUTTON_WIDTH / 2);

constexpr float TITLE_FONT_SIZE = 180.0f;

constexpr int DEFAULT_LAYER = Layers::UI + 10;
constexpr int BUTTON_LAYER = Layers::UI + 15;

class PauseMenuUI final : public UIObject
{
public:
    using resume_press_callback_t = std::function<void()>;
    using quit_press_callback_t = std::function<void()>;

    explicit PauseMenuUI(Scene& scene)
        : UIObject(scene, 1920, 1080, {0.0f, 0.0f}, {0.0f, 0.0f})
    {
        add_component<Image>("", false, false, 1920, 1080, Color{0, 0, 0, 128});
        layer(DEFAULT_LAYER);

        add_title();
        add_resume_button(scene);
        add_quit_button(scene);
    }

    void set_resume_press_callback(const resume_press_callback_t& resume_press_callback)
    {
        resume_press_callback_ = resume_press_callback;
    }

    void set_quit_press_callback(const quit_press_callback_t& quit_press_callback)
    {
        quit_press_callback_ = quit_press_callback;
    }

private:
    resume_press_callback_t resume_press_callback_;
    quit_press_callback_t quit_press_callback_;

    void add_title()
    {
        add_component<Text>(
                "Maybe him skill issue?",
                "ByteBounce",
                "resources/fonts/bytebounce/ByteBounce.ttf",
                TITLE_FONT_SIZE,
                Color{255, 255, 255, 255}
            )
            .alignment(Center)
            .offset(Point{0.0f, -BUTTON_HEIGHT * 3});
    }

    void add_resume_button(Scene& scene)
    {
        UIButton& btn = create_button(
            scene,
            "Back to game",
            BUTTON_START_X,
            BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 1,
            "button_large_green"
        );
        btn.parent(*this);
        btn.add_on_press([this](UIButton&) { resume_press_callback_(); });
    }

    void add_quit_button(Scene& scene)
    {
        UIButton& btn = create_button(
            scene,
            "Give up",
            BUTTON_START_X,
            BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * 2,
            "button_large_red"
        );
        btn.parent(*this);
        btn.add_on_press([this](UIButton&)
        {
            auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();
            if (multiplayer_service.get_connection_state() == ConnectionState::CONNECTED) {
                // TODO: CLEAN LEAVE, First send leave message and clean stuff. Then disconnect.
                
                if (multiplayer_service.get_peer_type() == PeerType::CLIENT) {
                    MsgUserLeave data{};
                    std::memcpy(&data, multiplayer_service.get_uuid().c_str(), sizeof(data));

                    Message msg = serialize_message(data, CustomMessageTypes::USER_LEAVE);
                    multiplayer_service.send(msg);
                }
                multiplayer_service.disconnect();
            } 
            else if (multiplayer_service.get_connection_state() != ConnectionState::NONE
                    && multiplayer_service.get_connection_state() != ConnectionState::DISCONNECTED) {
                return;
            }

            quit_press_callback_();

            auto& scene_service = Engine::instance().services->get_service<SceneService>().get();
            scene_service.load_scene("MainMenuScene");
        });
    }

    UIButton& create_button(
        Scene& scene,
        const std::string& label,
        float position_x,
        float position_y,
        const std::string& texture_id
    ) {
        auto& button = scene.add_game_object<UIButton>(
            scene,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            Point{0.5f, 0.5f},
            Point{0.5f, 0.5f},
            label,
            "ByteBounce",
            "resources/fonts/bytebounce/ByteBounce.ttf",
            texture_id
        );
        button.transform().position({position_x, position_y, 0.0f});
        button.label_color(Color{255, 255, 255, 255});
        button.font_size(BUTTON_FONT_SIZE);
        button.layer(BUTTON_LAYER);

        return button;
    }
};


