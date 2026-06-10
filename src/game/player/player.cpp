#include "player.hpp"

#include "manager/game_state.hpp"
#include "macros.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "player_alt_tex"
#include "player_tex"

constexpr float PLAYER_ALT_DRAW_SCALE = 0.68f;
constexpr float PLAYER_TOP_SPAWN_Y = 24.0f;

Player::Player() : tex(), texAlt(), x(), y(), Process(1000), lastShotPress() {}

float Player::centerX() const { return x + PLAYER_WIDTH / 2.0f; }

float Player::centerY() const { return y + PLAYER_HEIGHT / 2.0f; }

void Player::collisionRect(float &outX, float &outY, float &outW,
                           float &outH) const {
  outX = x;
  outY = y;
  outW = (float)PLAYER_WIDTH;
  outH = (float)PLAYER_HEIGHT;
}

void Player::prepareForNewRound() {
  healthPoints = PLAYER_MAX_HEALTH;
  lastShotPress = false;
  for (Cannonball &cannonball : cannonballs)
    cannonball.deactivate();
}

void Player::placeRandomTopSpawn() {
  y = PLAYER_TOP_SPAWN_Y;
  const int span =
      WINDOW_WIDTH > PLAYER_WIDTH ? WINDOW_WIDTH - PLAYER_WIDTH : 1;
  x = static_cast<float>(rand() % span);
}

void Player::takeDamage(int amount) {
  if (amount <= 0)
    return;
  if (healthPoints <= 0)
    return;
  healthPoints -= amount;
  if (healthPoints < 0)
    healthPoints = 0;
  if (healthPoints == 0)
    gameState.triggerGameOver();
}

int Player::health() const { return healthPoints; }

void Player::init() {
  tex = Texture(playerTexDat, playerTexWidth, playerTexHeight);
  texAlt = Texture(playerAltTexDat, playerAltTexWidth, playerAltTexHeight);
  if (!drawLinkedToMap) {
    addDependency((Drawable *)&gameState.map);
    drawLinkedToMap = true;
  }
  lastTime = NANOS;
}

void Player::run() {
  if (gameState.isGameOver()) {
    lastTime = NANOS;
    return;
  }

  if (gameState.paused) {
    lastTime = NANOS;
    return;
  }

  if (gameState.replay.isReplayActive()) {
    gameState.replay.syncReplay(x, y);
    lastTime = NANOS;
    return;
  }

  if (gameState.enemyStageTimeExpired()) {
    gameState.triggerGameOver();
    lastTime = NANOS;
    return;
  }

  dt = (NANOS - lastTime) / (float)NS;
  lastTime = NANOS;

  movement();

  const float pw = (float)PLAYER_WIDTH;
  const float ph = (float)PLAYER_HEIGHT;
  gameState.enemyManager.applyCollisionWithPlayer(x, y, pw, ph);
  gameState.allyManager.applyCollisionWithPlayer(x, y, pw, ph);

  gameState.replay.appendIfPlaying(static_cast<uint64_t>(NANOS) -
                                       gameState.replay.recordingTimeOrigin(),
                                   x, y);

  shooting();

  gameState.enemyManager.hitTrashByRect(x, y, (float)PLAYER_WIDTH,
                                        (float)PLAYER_HEIGHT);
  gameState.enemyManager.applyPlayerEnemyScorePenalty(
      x, y, (float)PLAYER_WIDTH, (float)PLAYER_HEIGHT);

  for (Cannonball &cannonball : cannonballs) {
    if (!cannonball.isActive())
      continue;

    float hitX = 0.0f;
    float hitY = 0.0f;
    float hitW = 0.0f;
    float hitH = 0.0f;
    cannonball.collisionRect(hitX, hitY, hitW, hitH);
    const float prevTop = hitY;

    cannonball.update(dt);

    cannonball.collisionRect(hitX, hitY, hitW, hitH);

    if (!gameState.isRectInVisionBox(hitX, hitY, hitW, hitH)) {
      cannonball.deactivate();
      continue;
    }

    const float sweepTop = std::min(prevTop, hitY);
    const float sweepBottom = std::max(prevTop + hitH, hitY + hitH);
    const float sweepH = sweepBottom - sweepTop;

    const int kills =
        gameState.enemyManager.hitEnemyByRect(hitX, sweepTop, hitW, sweepH, 1);
    if (kills > 0) {
      gameState.score += kills * SCORE_POINTS_ENEMY_KILL;
      cannonball.deactivate();
      continue;
    }
    if (gameState.enemyManager.cannonballDissolvesTrash(hitX, sweepTop, hitW,
                                                        sweepH)) {
      cannonball.deactivate();
      continue;
    }
    if (gameState.enemyManager.cannonballOverlapsDepositingEnemy(
            hitX, sweepTop, hitW, sweepH)) {
      takeDamage(PLAYER_DAMAGE_SHOOT_DEPOSITING_TRASH);
      cannonball.deactivate();
      continue;
    }
    if (gameState.allyManager.cannonballOverlapsAlly(hitX, sweepTop, hitW,
                                                    sweepH)) {
      gameState.subtractScoreBounded(SCORE_PENALTY_ALLY_HIT);
      cannonball.deactivate();
      continue;
    }
  }
}

void Player::movement() {
  if (gameState.replay.isReplayActive())
    return;

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

  const float pw = (float)PLAYER_WIDTH;
  const float ph = (float)PLAYER_HEIGHT;

  const float tryX = x + dx * dt;
  if (!gameState.enemyManager.playerRectBlocked(tryX, y, pw, ph) &&
      !gameState.allyManager.playerRectBlocked(tryX, y, pw, ph))
    x = tryX;

  const float tryY = y + dy * dt;
  if (!gameState.enemyManager.playerRectBlocked(x, tryY, pw, ph) &&
      !gameState.allyManager.playerRectBlocked(x, tryY, pw, ph))
    y = tryY;
}

void Player::shooting() {
  const bool *keyStates = SDL_GetKeyboardState(nullptr);
  const bool space = keyStates[SDL_SCANCODE_SPACE];

  if (feetOnBeach()) {
    lastShotPress = space;
    return;
  }

  if (space && !lastShotPress) {
    for (Cannonball &cannonball : cannonballs) {
      if (cannonball.isActive())
        continue;
      cannonball = Cannonball(x + CANNONBALL_OFFSET_X, y + CANNONBALL_OFFSET_Y,
                              BASE_CANNONBALL_SPEED);
      break;
    }
  }

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
    float crx = 0.f;
    float cry = 0.f;
    float crw = 0.f;
    float crh = 0.f;
    cannonball.collisionRect(crx, cry, crw, crh);
    if (!gameState.isRectInVisionBox(crx, cry, crw, crh))
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
