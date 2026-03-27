#include "map.hpp"
#include "manager/game_state.hpp"

#include "sand_tex"

#define SAND_TILE_SIZE 32

Map::Map(): beachRatio(), drawIndex() {

}

Map::Map(float beachRatio): beachRatio(beachRatio), drawIndex() {

}

void Map::init() {
    drawIndex = gameState.renderer.getNextFreeDrawIndex(std::bind(&Map::render, this, std::placeholders::_1));
    sandTex = Texture(sandTexDat, sandTexWidth, sandTexHeight);
}

void Map::run() {

}

void Map::destruct() {
    gameState.renderer.removeDrawIndex(drawIndex);
}

void Map::render(SDL_Surface* surface) {
    for(uint16_t y = WINDOW_HEIGHT; y > floor(beachRatio * WINDOW_HEIGHT); y -= SAND_TILE_SIZE) {
        for(uint16_t x = 0; x < WINDOW_WIDTH; x += SAND_TILE_SIZE) {
            sandTex.draw(surface,
                 {x, y, SAND_TILE_SIZE, SAND_TILE_SIZE}, {255, 255, 255, 255});
        }
    }
}