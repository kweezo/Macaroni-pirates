#include "game_state.hpp"

GameState gameState = GameState();

GameState::GameState() {

}

void GameState::start() {
    processManager.start();
    processManager.addProcess(*(Process*)&renderer);
    processManager.addProcess(*(Process*)&player);
}
