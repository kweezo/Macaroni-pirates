#include "game_state.hpp"

GameState gameState = GameState();

GameState::GameState(): map(0.5) {

}

void GameState::start() {
    processManager.start();
    processManager.addProcess(*(Process*)&renderer);
    processManager.addProcess(*(Process*)&player);
    processManager.addProcess(*(Process*)&map);
}
