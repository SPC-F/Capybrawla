#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/network/multiplayer_service.h>
#include <engine/audio/audio_service.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/ui/text.h>
#include <engine/public/ui/ui_image.h>
#include <engine/public/ui/ui_text.h>
#include <engine/public/ui/interactable/ui_button.h>
#include <engine/public/util/layers.h>

#include <game/scenes/lobby.h>
#include <game/character/player_object.h>
#include <game/network/message_types.h>

#include <iostream>

constexpr float BUTTON_WIDTH = 400.0f;
constexpr float BUTTON_HEIGHT = 80.0f;
constexpr float BUTTON_FONT_SIZE = 46.0f;

// float window_width = Engine::instance().services->get_service<RenderingService>().get().window().get_window_width();
// float window_height = Engine::instance().services->get_service<RenderingService>().get().window().get_window_height();
float window_width = 1920;
float window_height = 1080;

int max_players = 4;
float frame_width = window_width / 10;
float inner_frame_offset_x = frame_width / 2;
float content_width = frame_width * max_players + inner_frame_offset_x * (max_players - 1);
float outer_frame_offset_x = (window_width - content_width) / 2;
float offset_y = window_height / 5 * 2;
std::vector<std::string> colors = {"yellow", "red", "blue", "green"};
std::vector<std::string> colors_2 = {"default", "red", "blue", "green"};

LobbyScene::LobbyScene() : Level("Level_LobbyScene"), multiplayer_controller_{create_multiplayer_controller()} {}

void LobbyScene::setup(Scene& scene) {

}

void LobbyScene::load(Scene& scene) {
    LobbyScene::load_camera();

    GameObject& bg = scene.add_game_object("Background");
    bg.add_component<Sprite>("main_menu_bg", Color{255, 255, 255, 255}, 0, 0, 0, 0);
    bg.transform().position({0, 0, 0});

    auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();

    if (multiplayer_service.get_connection_state() == ConnectionState::NONE
        && multiplayer_service.get_peer_type() == PeerType::HOST) {
        multiplayer_service.set_max_clients(4);
        multiplayer_service.set_connection_port(1024);
        multiplayer_service.start_server();

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();

            controller.register_user(multiplayer_service.get_uuid());
        }
    }

    if (multiplayer_service.get_peer_type() == PeerType::HOST)
        register_host_handlers(multiplayer_service);
    else if (multiplayer_service.get_peer_type() == PeerType::CLIENT)
        register_client_handlers(multiplayer_service);

    create_ip_text();
    create_player_displays(multiplayer_service);
    create_start_button();
}

GameObject& LobbyScene::create_multiplayer_controller() {
    auto& obj = scene().add_game_object("MultiplayerController");
    obj.add_component<BehaviorScript>(std::make_unique<MultiplayerController>());
    
    return obj;
}

void LobbyScene::register_host_handlers(MultiplayerService& multiplayer_service) {
    multiplayer_service.register_handler(CustomMessageTypes::USER_JOIN, [&multiplayer_service, this](const Message& message) {
        MsgUserJoin data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        std::cout << "New user joined with UUID " << data.uuid << std::endl;
        Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
        multiplayer_service.send(msg);

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();

            controller.register_user(data.uuid);
            update_player_displays(multiplayer_service);
        }
    });
    
    multiplayer_service.register_handler(CustomMessageTypes::USER_LEAVE, [&multiplayer_service, this](const Message& message) {
        MsgUserLeave data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        std::cout << "User with UUID " << data.uuid << " left" << std::endl;
        Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
        multiplayer_service.send(msg);

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();

            controller.unregister_user(data.uuid);
            update_player_displays(multiplayer_service);
        }
    });
}

void LobbyScene::register_client_handlers(MultiplayerService& multiplayer_service) {
    if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
        auto& controller = controller_opt.value().get();

        controller.on_connection_state_change([&multiplayer_service, this](ConnectionState old_state, ConnectionState new_state) {
            if (new_state == ConnectionState::CONNECTED) {
                std::cout << "Connected with UUID " << multiplayer_service.get_uuid() << std::endl;

                MsgUserJoin data{};
                std::strncpy(data.uuid, multiplayer_service.get_uuid().c_str(), sizeof(data.uuid) - 1);

                Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
                multiplayer_service.send(msg);
            }
        });
    }
}

void LobbyScene::create_ip_text() {
    std::string host_ip = "PLACEHOLDER IP";

    auto& player_text = player_name(scene(), host_ip, frame_width, 128);
    player_text.transform().local_position({(window_width - frame_width) / 2, window_height / 7, 0});
}

void LobbyScene::update_player_displays(MultiplayerService& multiplayer_service) {
    for (auto frame : player_frames_) {
        scene().remove_game_object(*frame);
    }
    player_frames_.clear();
    
    create_player_displays(multiplayer_service);
}

void LobbyScene::create_player_displays(MultiplayerService& multiplayer_service) {
    std::map<UserColor, std::string> users;

    if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
        auto& controller = controller_opt.value().get();

        users = controller.users();
    }

    for (int i = 0; i < max_players; ++i) {
        float offset = outer_frame_offset_x + (frame_width + inner_frame_offset_x) * i;

        bool has_user = users.find(static_cast<UserColor>(i)) != users.end();

        if (has_user) {
            auto& player_text = player_name(scene(), "PLACEHOLDER", frame_width, 48);
            player_text.transform().local_position({offset, offset_y - 100, 0});
            player_frames_.push_back(&player_text);

            auto &player_background = scene().add_game_object<UIImage>(scene(), "button_small_" + colors[i], frame_width, frame_width, Point{}, Point{});
            player_background.transform().local_position({offset, offset_y, 0});
            player_frames_.push_back(&player_background);

            auto &player_skin_image = scene().add_game_object<UIImage>(scene(), "capybara_" + colors_2[i] + "_idle", frame_width, frame_width, Point{}, Point{});
            player_skin_image.transform().local_position({offset, offset_y - 10, 0});
            player_frames_.push_back(&player_skin_image);
        } else {
            auto &player_background = scene().add_game_object<UIImage>(scene(), "button_small_black", frame_width, frame_width, Point{}, Point{});
            player_background.transform().local_position({offset, offset_y, 0});
            player_frames_.push_back(&player_background);
        }
    }
}

void LobbyScene::create_start_button() {
    auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();

    std::string start_btn_txt = multiplayer_service.get_peer_type() == PeerType::HOST ? "Start game" : "Waiting for Host";
    UIButton& start_button = create_button(
        scene(),
        start_btn_txt,
        (window_width - BUTTON_WIDTH) / 2,
        window_height / 4 * 3,
        "button_large_red"
    );
    // start_button.parent(parent);
    if (multiplayer_service.get_peer_type() == PeerType::HOST) {
        start_button.add_on_press([this](UIButton& /*btn*/) {
            
        });
    } else {
        start_button.disable();
    }
}

UIButton& LobbyScene::create_button(
    Scene& scene,
    const std::string& label, 
    float position_x,
    float position_y, 
    const std::string& texture_id
) {
    UIButton& button = scene.add_game_object<UIButton>(
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
    button.layer(Layers::UI);

    button.add_on_hover([] (UIInteractable&) {
      auto& audio_service = Engine::instance().services->get_service<AudioService>().get();
      audio_service.play_sound("btn_hover", 0.1f);
    });

    return button;
}

UIText& LobbyScene::player_name(Scene &scene, const std::string& username, float frame_width, float font_size) {
    UIText &player_name = scene.add_game_object<UIText>(
        scene,
        username,
        "ByteBounce",
        "resources/fonts/bytebounce/ByteBounce.ttf",
        frame_width,
        BUTTON_HEIGHT,
        Point{},
        Point{});

    player_name.font_size(font_size);
    return player_name;
}