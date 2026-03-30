#include "player.hpp"

#include "manager/game_state.hpp"

#include "player_tex"

Player::Player(): tex(), x(), y(), Process(1000), lastShotPress() {
}

void Player::init() {
    tex = Texture(playerTexDat, playerTexWidth, playerTexHeight);
    addDependency((Drawable*)&gameState.map);
}

void Player::run() {
    movement();
    shooting();

    int i = 0;
    for(Cannonball& cannonball : cannonballs) {
        if(!cannonball.isActive()) continue;

        cannonball.update(dt);
        i++;
    }

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

void Player::shooting() {
    const bool* keyStates = SDL_GetKeyboardState(nullptr);

    if(!keyStates[SDL_SCANCODE_SPACE] || lastShotPress)
        goto player_shoot_end;
    
    for(Cannonball& cannonball : cannonballs) {
        if(cannonball.isActive()) continue;

        cannonball = Cannonball(x + CANNONBALL_OFFSET_X, y + CANNONBALL_OFFSET_Y, BASE_CANNONBALL_SPEED);
        break;
    }

player_shoot_end:
    lastShotPress = keyStates[SDL_SCANCODE_SPACE];
}

void Player::destruct() {

}

void Player::render(SDL_Surface* surface) {
    for(Cannonball& cannonball : cannonballs) {
        if(!cannonball.isActive()) continue;


        cannonball.draw(surface);
    }

    tex.draw(surface,
    {(int)floor(x), (int)floor(y), 150, 100},
    {255, 255, 255, 255});
}