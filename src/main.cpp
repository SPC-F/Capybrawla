#include <iostream>

#include <engine/util/memory.h>
#include <engine/core/engine.h>

namespace {
    #ifdef TRACY_ENABLE
    inline void tracy_init() { tracy_memory_init(); }
    inline void tracy_shutdown() { tracy_memory_shutdown(); }
    #else
    inline void tracy_init() {}
    inline void tracy_shutdown() {}
    #endif
}

void run() {
    Engine& engine = Engine::instance();

    std::cout << "Capybrawla!" << std::endl;

    engine.quit();
}

int main() {
    tracy_init();
    
    try {
       run(); 
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }

    tracy_shutdown();

    return 0;
}