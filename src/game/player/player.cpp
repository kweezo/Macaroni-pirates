#include "player.hpp"

#include "manager/game_state.hpp"

#include "player_tex"

Player::Player(): tex(), drawIndex(), x(), y(), Process(1000) {
}

void Player::init() {
    tex = Texture(texDat, texWidth, texHeight);
    drawIndex = gameState.renderer.getNextFreeDrawIndex(std::bind(&Player::render, this, std::placeholders::_1));
}

void Player::run() {
    movement();

    dt = (MILLIS - lastTime) / (float)NS;
    lastTime = MILLIS;
}


void Player::movement() {
    float dx = 0, dy = 0;

    const bool* keyStates = SDL_GetKeyboardState(nullptr);

    if(keyStates[SDL_SCANCODE_W]) {
        dy -= 1;
    }
    if(keyStates[SDL_SCANCODE_S]) {
        dy += 1;
    }
    if(keyStates[SDL_SCANCODE_D]) {
        dx += 1;
    }
    if(keyStates[SDL_SCANCODE_A]) {
        dx -= 1;
    }

    float scalar = sqrt(pow(dx, 2) + pow(dy, 2));
    if(!dx && !dy) scalar = 1;

    dx *= speed / scalar;
    dy *= speed / scalar;

    x += dx * dt;
    y += dy * dt;
}

void Player::destruct() {
    gameState.renderer.removeDrawIndex(drawIndex);
}

void Player::render(SDL_Surface* surface) {
    tex.draw(surface,
    {(int)floor(x), (int)floor(y), 150, 100},
    {255, 255, 255, 255});
}