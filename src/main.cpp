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

int main() {
    //tracy_init();
    
    Game game;
    try {
        game.initialize();
        game.run();
        game.shutdown();
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }

    //tracy_shutdown();

    return 0;
}