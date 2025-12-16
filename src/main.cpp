#include <engine/util/memory.h>

#include <../include/game/game.h>

#include <exception>
#include <iostream>

// namespace {
//     #ifdef TRACY_ENABLE
//     inline void tracy_init() { tracy_memory_init(); }
//     inline void tracy_shutdown() { tracy_memory_shutdown(); }
//     #else
//     inline void tracy_init() {}
//     inline void tracy_shutdown() {}
//     #endif
// }

int main() {
    //tracy_init();
    
    try {
        Game::initialize();
        Game::run();
        Game::shutdown();
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }

    //tracy_shutdown();

    return 0;
}