#include "game/ally/ally.hpp"

#include "manager/game_state.hpp"

AllyManager::AllyManager() : Process(8000) {}

void AllyManager::init() { addDependency((Drawable *)&gameState.map); }

void AllyManager::run() {}

void AllyManager::destruct() {}

bool AllyManager::onCannonballHit(float, float, float, float) { return false; }

void AllyManager::render(SDL_Surface *) {}
