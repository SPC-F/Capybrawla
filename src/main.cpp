#include <exception>
#include <iostream>

#include <game/game.h>

// #include <engine/util/memory.h>
// namespace {
//     #ifdef TRACY_ENABLE
//     inline void tracy_init() { tracy_memory_init(); }
//     inline void tracy_shutdown() { tracy_memory_shutdown(); }
//     #else
//     inline void tracy_init() {}
//     inline void tracy_shutdown() {}
//     #endif
// }

constexpr bool FULLSCREEN_DEFAULT = true;
constexpr int WINDOW_WIDTH_DEFAULT = 1280;
constexpr int WINDOW_HEIGHT_DEFAULT = 720;

/// A simple command-line argument parser
/// The following arguments are supported:
/// --fullscreen            Launch the game in fullscreen mode (default)
/// --resize                Launch the game in windowed mode
/// --w <width>             Set the window width (only if --resize is used)
/// --h <height>            Set the window height (only if --resize is used)
void load_arguments(int argc, char** argv, bool& fullscreen, int& width, int& height) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--resize")                  fullscreen = false;
        else if (arg == "--fullscreen")         fullscreen = true;
        else if (arg == "--w" && i + 1 < argc)  width = std::stoi(argv[++i]);
        else if (arg == "--h" && i + 1 < argc)  height = std::stoi(argv[++i]);
    }
}

int main() {
    //tracy_init();

    bool fullscreen = FULLSCREEN_DEFAULT;
    int width = WINDOW_WIDTH_DEFAULT;
    int height = WINDOW_HEIGHT_DEFAULT;

    int argc = 0;
    char** argv = nullptr;

    /// Cross-platform argc and argv
    #ifdef _WIN32
        argc = __argc;
        argv = __argv;
    #else
        // extern int argc;
        // extern char** argv;
    #endif

    load_arguments(argc, argv, fullscreen, width, height);

    Game game;
    try {
        game.initialize();

        if (fullscreen)     game.set_fullscreen();
        else                game.set_resizable(static_cast<float>(width), static_cast<float>(height));
        game.run();
        
        game.shutdown();
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }

    //tracy_shutdown();
    return 0;
}