#include "enemy.hpp"

#include "enemy_tex"
#include "enemy_inv_tex"

#include "manager/game_state.hpp"

EnemyManager::EnemyManager(): Process(5000) {

}

void EnemyManager::init() {
    lastTime = NANOS; //to prevent the multiplication catastrophe of '26 from happening again
    tex = Texture(enemyTexDat, enemyTexWidth, enemyTexHeight);
    invTex = Texture(enemyInvTexDat, enemyInvTexWidth, enemyInvTexHeight);
    addDependency((Drawable*)&gameState.map);

    spawnInstances(10);
}

void EnemyManager::run() {
    for(Instance& instance : instances) {
        if(!instance.active) continue;
        updateInstance(instance);
    }

    dt = (NANOS - lastTime) / (float)NS;
    lastTime = NANOS;
}

void EnemyManager::destruct() {

}

void EnemyManager::render(SDL_Surface* surface) {
    for(Instance& instance : instances) {
        if(!instance.active) continue;
        
        if(instance.invulnerable) {
            invTex.draw(surface,
            {(uint16_t)instance.x, (uint16_t)instance.y, ENEMY_SIZE, ENEMY_SIZE},
            {255, 200, 200});
        } else {
            tex.draw(surface,
            {(uint16_t)instance.x, (uint16_t)instance.y, ENEMY_SIZE, ENEMY_SIZE},
            {255, 255, 255});
        }
    }
}

bool EnemyManager::isInstanceDepositing(Instance& instance) {
    return NANOS - instance.oceanTouchTime < ENEMY_TRASH_DEPOSIT_TIME;
}

void EnemyManager::updateInstance(Instance& instance) {
    instance.invulnerable = false;

    if(isInstanceDepositing(instance)) {
        if(instance.leftNeigbour != -1 && isInstanceDepositing(instances[instance.leftNeigbour]))
            instance.invulnerable = true;
        if(instance.rightNeighbour != -1 && isInstanceDepositing(instances[instance.rightNeighbour]))
            instance.invulnerable = true;
        return;
    }

    if(instance.dir) {
        instance.y += instance.speed * dt;
    } else {
        instance.y -= instance.speed * dt;
    }

    if(!gameState.map.onBeach(instance.x, instance.y) && !instance.dir) {
        instance.dir = true;
        instance.oceanTouchTime = NANOS;
    }

    if(instance.y > WINDOW_HEIGHT - ENEMY_SIZE)
        instance.dir = false;


}

void EnemyManager::spawnInstances(size_t count) {
    assert(count <= MAX_ENEMY_COUNT);
    const uint32_t INTERVAL = WINDOW_WIDTH / MAX_ENEMY_COUNT; // we spawn enemies in slices
    std::array<size_t, MAX_ENEMY_COUNT> filledSlots;
    filledSlots.fill(-1);

    uint32_t enemiesSpawned = 0;

    for(int x = 0; enemiesSpawned != count; x += INTERVAL) {
        if(x >= WINDOW_WIDTH) x = 0;
        if(rand() % MAX_ENEMY_COUNT || filledSlots[x / INTERVAL] != -1) continue;

        instances[enemiesSpawned].x = x;
        instances[enemiesSpawned].y = WINDOW_HEIGHT - ENEMY_SIZE;
        instances[enemiesSpawned].active = true;
        instances[enemiesSpawned].speed = BASE_ENEMY_SPEED + (rand() % 100 - 50) / 100.0f * ENEMY_SPEED_DEVIATION;

        filledSlots[x / INTERVAL] = enemiesSpawned;

        enemiesSpawned++;
    }

    for(Instance& instance : instances) {
        if(!instance.active) continue;

        if(instance.x / INTERVAL != 0)
            instance.leftNeigbour = filledSlots[instance.x / INTERVAL - 1];
        if(instance.x / INTERVAL < MAX_ENEMY_COUNT - 1)
            instance.rightNeighbour = filledSlots[instance.x / INTERVAL + 1];
    }
}