#pragma once

/**
 * @brief The Game class manages the main game lifecycle.
 * 
 * It provides static methods to initialize, run, and shut down the game.
 */
class Game {
public:
    static void initialize();
    static void run();
    static void shutdown();
};

