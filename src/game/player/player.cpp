#include "player.hpp"

#include "manager/game_state.hpp"

#include <cmath>

#include "player_alt_tex"
#include "player_tex"

namespace {
constexpr float PLAYER_ALT_DRAW_SCALE = 0.68f;
}

Player::Player() : tex(), texAlt(), x(), y(), Process(1000), lastShotPress() {}

float Player::centerX() const { return x + PLAYER_WIDTH / 2.0f; }

float Player::centerY() const { return y + PLAYER_HEIGHT / 2.0f; }

void Player::init() {
  tex = Texture(playerTexDat, playerTexWidth, playerTexHeight);
  texAlt = Texture(playerAltTexDat, playerAltTexWidth, playerAltTexHeight);
  addDependency((Drawable *)&gameState.map);
  lastTime = NANOS;
}

void Player::run() {
  if (gameState.paused.load(std::memory_order_relaxed)) {
    lastTime = NANOS;
    return;
  }

  if (gameState.replay.isReplayActive()) {
    gameState.replay.syncReplay(x, y);
    lastTime = NANOS;
    return;
  }

  dt = (NANOS - lastTime) / (float)NS;
  lastTime = NANOS;

  movement();

  gameState.replay.appendIfPlaying(static_cast<uint64_t>(NANOS) -
                                       gameState.replay.recordingTimeOrigin(),
                                   x, y);

  {
    std::lock_guard<std::mutex> guard(gameState.worldSimMutex);
    gameState.enemyManager.hitTrashByRect(x, y, (float)PLAYER_WIDTH,
                                          (float)PLAYER_HEIGHT);
  }

  shooting();

  int i = 0;
  for (Cannonball &cannonball : cannonballs) {
    if (!cannonball.isActive())
      continue;

    cannonball.update(dt);
    float hitX = 0.0f;
    float hitY = 0.0f;
    float hitW = 0.0f;
    float hitH = 0.0f;
    cannonball.collisionRect(hitX, hitY, hitW, hitH);
    if (gameState.enemyManager.hitEnemyByRect(hitX, hitY, hitW, hitH, 1)) {
      cannonball.deactivate();
      continue;
    }
    if (gameState.allyManager.onCannonballHit(hitX, hitY, hitW, hitH)) {
      cannonball.deactivate();
      continue;
    }
    i++;
  }
}

void Player::movement() {
  float dx = 0, dy = 0;

  const bool *keyStates = SDL_GetKeyboardState(nullptr);

  if (keyStates[SDL_SCANCODE_W]) {
    dy -= 1;
  }
  if (keyStates[SDL_SCANCODE_S]) {
    dy += 1;
  }
  if (keyStates[SDL_SCANCODE_D]) {
    dx += 1;
  }
  if (keyStates[SDL_SCANCODE_A]) {
    dx -= 1;
  }

  float scalar = sqrt(pow(dx, 2) + pow(dy, 2));
  if (!dx && !dy)
    scalar = 1;

  dx *= speed / scalar;
  dy *= speed / scalar;

  x += dx * dt;
  y += dy * dt;
}

void Player::shooting() {
  const bool *keyStates = SDL_GetKeyboardState(nullptr);
  const bool space = keyStates[SDL_SCANCODE_SPACE];

  if (feetOnBeach()) {
    lastShotPress = space;
    return;
  }

  if (!space || lastShotPress)
    goto player_shoot_end;

  for (Cannonball &cannonball : cannonballs) {
    if (cannonball.isActive())
      continue;

    cannonball = Cannonball(x + CANNONBALL_OFFSET_X, y + CANNONBALL_OFFSET_Y,
                            BASE_CANNONBALL_SPEED);
    break;
  }

player_shoot_end:
  lastShotPress = space;
}

bool Player::feetOnBeach() const {
  return gameState.map.onBeach(centerX(), y + PLAYER_HEIGHT * 0.92f);
}

void Player::destruct() {}

void Player::render(SDL_Surface *surface) {
  for (Cannonball &cannonball : cannonballs) {
    if (!cannonball.isActive())
      continue;
    cannonball.draw(surface);
  }

  if (feetOnBeach()) {
    const int aw = (int)std::floor((float)PLAYER_WIDTH * PLAYER_ALT_DRAW_SCALE);
    const int ah =
        (int)std::floor((float)PLAYER_HEIGHT * PLAYER_ALT_DRAW_SCALE);
    const int ox =
        (int)std::floor(x + ((float)PLAYER_WIDTH - (float)aw) * 0.5f);
    const int oy = (int)std::floor(y + (float)PLAYER_HEIGHT - (float)ah);
    texAlt.drawSprite(surface, {ox, oy, aw, ah}, {255, 255, 255, 255});
  } else {
    tex.drawSprite(
        surface,
        {(int)std::floor(x), (int)std::floor(y), PLAYER_WIDTH, PLAYER_HEIGHT},
        {255, 255, 255, 255});
  }
}
