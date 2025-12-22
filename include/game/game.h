#pragma once

#include <game/scenes/level.h>
#include <engine/public/scene.h>

/**
 * @brief The Game class manages the main game lifecycle.
 * 
 * It provides static methods to initialize, run, and shut down the game.
 */
class Game {
public:
    void initialize();
    void run();
    void shutdown();
private:
    std::vector<std::unique_ptr<Level>> levels_;

    void bootstrap(Scene& first_scene);
};

