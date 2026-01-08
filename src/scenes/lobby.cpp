#include <engine/core/engine.h>
#include <engine/core/rendering/renderingService.h>
#include <engine/network/multiplayer_service.h>
#include <engine/audio/audio_service.h>
#include <engine/storage/simple_storage.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/ui/text.h>
#include <engine/public/ui/ui_image.h>
#include <engine/public/ui/ui_text.h>
#include <engine/public/ui/interactable/ui_button.h>
#include <engine/public/util/layers.h>
#include <engine/public/scene_service.h>

#include <game/scenes/lobby.h>
#include <game/character/player_object.h>
#include <game/network/message_types.h>

#include <iostream>

constexpr float BUTTON_WIDTH = 400.0f;
constexpr float BUTTON_HEIGHT = 80.0f;
constexpr float BUTTON_FONT_SIZE = 46.0f;

constexpr float WINDOW_WIDTH = 1920;
constexpr float WINDOW_HEIGHT = 1080;

constexpr int MAX_PLAYERS = 4;
constexpr float FRAME_WIDTH = WINDOW_WIDTH / 10;
constexpr float INNER_FRAME_OFFSET_X = FRAME_WIDTH / 2;
constexpr float CONTENT_WIDTH = FRAME_WIDTH * MAX_PLAYERS + INNER_FRAME_OFFSET_X * (MAX_PLAYERS - 1);
constexpr float OUTER_FRAME_OFFSET_X = (WINDOW_WIDTH - CONTENT_WIDTH) / 2;
constexpr float OFFSET_Y = WINDOW_HEIGHT / 5 * 2;

LobbyScene::LobbyScene() : Level("Level_LobbyScene"), multiplayer_controller_{create_multiplayer_controller()} {}

void LobbyScene::setup(Scene& scene) {
    AudioService &audio_service = Engine::instance().services->get_service<AudioService>().get();
    scene.on_run([&audio_service](Scene& scene) {
        audio_service.play_sound("sans", 0.05f, true);
    });

    scene.on_stop([&audio_service](Scene& scene) {
        audio_service.stop_all_sounds();
    });

    multiplayer_controller_.mark_dont_destroy_on_load(true);
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
            std::string username = SimpleStorage::instance().get_value_or_default<std::string>("username", "PLACEHOLDER");

            controller.register_user(multiplayer_service.get_uuid(), username);
        }
    }

    if (multiplayer_service.get_peer_type() == PeerType::HOST)
        register_host_handlers(multiplayer_service);
    else if (multiplayer_service.get_peer_type() == PeerType::CLIENT)
        register_client_handlers(multiplayer_service);

    create_ip_text(multiplayer_service);
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

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();

            controller.register_user(data.uuid, data.name);
            update_player_displays(multiplayer_service);
            
            MsgLobbyData lobby_data{};
            lobby_data.user_count = 0;
            for (auto [key, value] : controller.users()) {
                LobbyUserData user_data{};
                std::strncpy(user_data.uuid, value.uuid.c_str(), sizeof(user_data.uuid) - 1);
                user_data.color = static_cast<uint16_t>(value.color);
                std::strncpy(user_data.name, value.username.c_str(), sizeof(user_data.name) - 1);

                lobby_data.users[lobby_data.user_count] = user_data;
                ++lobby_data.user_count;
            }

            Message lobby_msg = serialize_message(lobby_data, CustomMessageTypes::LOBBY_DATA);
            multiplayer_service.send_to_peer_via_uuid(data.uuid, lobby_msg);
        }

        Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
        multiplayer_service.send(msg);
    });
    
    multiplayer_service.register_handler(CustomMessageTypes::USER_LEAVE, [&multiplayer_service, this](const Message& message) {
        MsgUserLeave data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        Message msg = serialize_message(data, CustomMessageTypes::USER_LEAVE);
        multiplayer_service.send(msg);

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();

            controller.unregister_user(data.uuid);
            update_player_displays(multiplayer_service);
        }
    });
}

void LobbyScene::register_client_handlers(MultiplayerService& multiplayer_service) {
    multiplayer_service.register_handler(CustomMessageTypes::LOBBY_DATA, [&multiplayer_service, this](const Message& message) {
        MsgLobbyData data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();
            
            for (uint32_t i = 0; i < data.user_count; ++i) {
                LobbyUserData user_data{};
                std::memcpy(&user_data, &data.users[i], sizeof(user_data));
                
                controller.register_user(user_data.uuid, user_data.name, static_cast<UserColor>(user_data.color));
            }
            
            update_player_displays(multiplayer_service);
        }
    });

    multiplayer_service.register_handler(CustomMessageTypes::USER_JOIN, [&multiplayer_service, this](const Message& message) {
        MsgUserJoin data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        if (data.uuid == multiplayer_service.get_uuid()) return;

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();

            controller.register_user(data.uuid, data.name);
            update_player_displays(multiplayer_service);
        }
    });
    
    multiplayer_service.register_handler(CustomMessageTypes::USER_LEAVE, [&multiplayer_service, this](const Message& message) {
        MsgUserLeave data{};
        std::memcpy(&data, message.payload.data(), sizeof(data));

        if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
            auto& controller = controller_opt.value().get();

            controller.unregister_user(data.uuid);
            update_player_displays(multiplayer_service);
        }
    });

    multiplayer_service.register_handler(CustomMessageTypes::ROUND_START, [&multiplayer_service, this](const Message& message) {
        auto& scene_service = Engine::instance().services->get_service<SceneService>().get();
        scene_service.load_scene("Level_SwampAutumScene");
    });

    if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
        auto& controller = controller_opt.value().get();

        controller.on_connection_state_change([&multiplayer_service, this](ConnectionState old_state, ConnectionState new_state) {
            if (new_state == ConnectionState::CONNECTED) {
                MsgUserJoin data{};
                std::strncpy(data.uuid, multiplayer_service.get_uuid().c_str(), sizeof(data.uuid) - 1);
                std::string username = SimpleStorage::instance().get_value_or_default<std::string>("username", "PLACEHOLDER");
                std::strncpy(data.name, username.c_str(), sizeof(data.name) - 1);

                Message msg = serialize_message(data, CustomMessageTypes::USER_JOIN);
                multiplayer_service.send(msg);
            }
        });
    }
}

void LobbyScene::create_ip_text(MultiplayerService& multiplayer_service) {
    std::string host_ip = multiplayer_service.get_host_ip();

    auto& player_text = player_name(scene(), host_ip, FRAME_WIDTH, 128);
    player_text.transform().local_position({(WINDOW_WIDTH - FRAME_WIDTH) / 2, WINDOW_HEIGHT / 7, 0});
}

void LobbyScene::update_player_displays(MultiplayerService& multiplayer_service) {
    for (auto frame : player_frames_) {
        scene().remove_game_object(frame.get());
    }
    player_frames_.clear();
    
    create_player_displays(multiplayer_service);
}

void LobbyScene::create_player_displays(MultiplayerService& multiplayer_service) {
    std::map<UserColor, User> users;

    if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
        auto& controller = controller_opt.value().get();

        users = controller.users();
    }

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        float offset = OUTER_FRAME_OFFSET_X + (FRAME_WIDTH + INNER_FRAME_OFFSET_X) * i;

        bool has_user = users.find(static_cast<UserColor>(i)) != users.end();

        if (has_user) {
            std::vector<std::string> btn_colors = {"yellow", "red", "blue", "green"};
            std::vector<std::string> skin_colors = {"default", "red", "blue", "green"};

            auto& player_text = player_name(scene(), users[static_cast<UserColor>(i)].username, FRAME_WIDTH, 48);
            player_text.transform().local_position({offset, OFFSET_Y - 100, 0});
            player_frames_.push_back(std::ref(player_text));

            auto &player_background = scene().add_game_object<UIImage>(scene(), "button_small_" + btn_colors[i], FRAME_WIDTH, FRAME_WIDTH, Point{}, Point{});
            player_background.transform().local_position({offset, OFFSET_Y, 0});
            player_frames_.push_back(std::ref(player_background));

            auto &player_skin_image = scene().add_game_object<UIImage>(scene(), "capybara_" + skin_colors[i] + "_idle", FRAME_WIDTH, FRAME_WIDTH, Point{}, Point{});
            player_skin_image.transform().local_position({offset, OFFSET_Y - 10, 0});
            player_frames_.push_back(std::ref(player_skin_image));
        } else {
            auto &player_background = scene().add_game_object<UIImage>(scene(), "button_small_black", FRAME_WIDTH, FRAME_WIDTH, Point{}, Point{});
            player_background.transform().local_position({offset, OFFSET_Y, 0});
            player_frames_.push_back(std::ref(player_background));
        }
    }
}

void LobbyScene::create_start_button() {
    auto& multiplayer_service = Engine::instance().services->get_service<MultiplayerService>().get();

    std::string start_btn_txt = multiplayer_service.get_peer_type() == PeerType::HOST ? "Start game" : "Waiting for Host";
    UIButton& start_button = create_button(
        scene(),
        start_btn_txt,
        (WINDOW_WIDTH - BUTTON_WIDTH) / 2,
        WINDOW_HEIGHT / 4 * 3,
        "button_large_red"
    );

    if (multiplayer_service.get_peer_type() == PeerType::HOST) {
        start_button.add_on_press([this](UIButton& /*btn*/) {
            if (auto controller_opt = multiplayer_controller_.get_script<BehaviorScript, MultiplayerController>(); controller_opt.has_value()) {
                auto& controller = controller_opt.value().get();

                if (controller.users().size() < 2) return;

                MsgRoundStart data{};
                Message msg = serialize_message(data, CustomMessageTypes::ROUND_START);
                Engine::instance().services->get_service<MultiplayerService>().get().send(msg);

                auto& scene_service = Engine::instance().services->get_service<SceneService>().get();
                scene_service.load_scene("Level_SwampAutumScene");
            }
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