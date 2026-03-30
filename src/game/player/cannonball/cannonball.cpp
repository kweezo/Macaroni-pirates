#include "cannonball.hpp"
#include "cannonball_tex"

Cannonball::Cannonball(): x(), y(), speed(), active(false) {

}

Cannonball::Cannonball(float x, float y, float speed): x(x), y(y), speed(speed),
 tex(cannonballTexDat, cannonballTexWidth, cannonballTexHeight), active(true) {

}

void Cannonball::update(float dt)  {
    movement(dt);
    boundsCheck();
}

void Cannonball::movement(float dt) {
    y += speed * dt;
}

void Cannonball::boundsCheck() {
    if(y > WINDOW_HEIGHT)
        active = false;
}

void Cannonball::draw(SDL_Surface* surface) {
    tex.draw(surface, {(int)x, (int)y, cannonballTexWidth, cannonballTexHeight}, {255, 255, 255});
}

bool Cannonball::isActive() {
    return active;
}