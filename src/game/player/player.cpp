#include "player.hpp"

#include "manager/game_state.hpp"

Player::Player(): tex(), drawIndex(), x(), y() {

}

void Player::init() {
    static uint8_t color[]  = {127, 127, 127};
    tex = Texture(color, 1, 1);
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
    std::cerr << dt << "\n";

    x += dx * dt;
    y += dy * dt;
}

void Player::destruct() {
    gameState.renderer.removeDrawIndex(drawIndex);
}

void Player::render(SDL_Renderer* renderer) {
    tex.draw(renderer,
    {(uint16_t)floor(x), (uint16_t)floor(y), 100, 100},
    {255, 255, 255, 255});
}