#pragma once

#include "manager/process/process.hpp"
#include "renderer/drawable/drawable.hpp"
#include "texture/texture.hpp"

#include <bitset>
#include <cassert>
#include <array>

#define BASE_ENEMY_SPEED 100
#define ENEMY_SPEED_DEVIATION 50
#define MAX_ENEMY_COUNT 20
#define ENEMY_SIZE 64
#define ENEMY_TRASH_DEPOSIT_TIME (2 * NS) // in seconds

class EnemyManager: public Process, Drawable {
    struct Instance {
        size_t oceanTouchTime;
        uint32_t leftNeigbour, rightNeighbour;
        float x, y;
        float speed;
        bool dir;
        bool active;
        bool invulnerable;
    };

    public:
        EnemyManager();
        bool hitEnemyByRect(float x, float y, float w, float h, int maxHits);
        void hitTrashByRect(float x, float y, float w, float h);
    private:
        void init() override;
        void run() override;
        void destruct() override;

        void spawnInstances(size_t count);

        void updateInstance(Instance& instance);
        inline bool isInstanceDepositing(Instance& instance);

        void render(SDL_Surface* surface) override;

        Texture tex;
        Texture invTex;

        std::array<Instance, MAX_ENEMY_COUNT> instances;

        float dt;
        size_t lastTime;
};