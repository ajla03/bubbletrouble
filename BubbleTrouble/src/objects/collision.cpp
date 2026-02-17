#include "resources.h"
#include "gameContext.h"
#include "game.h"
#include "constants.h"
#include <windows.h>
#include "database.h"
#include <cmath>
#include <algorithm>

// Moved from input.cpp
bool AABB(float ax, float ay, float aw, float ah,
          float bx, float by, float bw, float bh)
{
    return (ax + aw > bx) &&
           (ax < bx + bw) &&
           (ay + ah > by) &&
           (ay < by + bh);
}

// Moved from input.cpp
void CheckHeroDoorCollision()
{
    if(!CURRENT_LEVEL.door.active) return;

    StaticObject* d = &CURRENT_LEVEL.door;
    Hero* h = &gGame.hero;

    if (d->width <= 0 || d->height <= 0)
        return;

    if (AABB(h->x, h->y, h->width, h->height,
             d->x, d->y, d->width, d->height))
    {
        if (h->x < d->x)
            h->x = d->x - h->width;
        else
            h->x = d->x + d->width;
        return;
    }
}

// Moved from input.cpp
void CheckHeroPillarCollision(StaticObject* pillar)
{
    if(pillar->width <= 0 || pillar->height <= 0) return;

    Hero* h = &gGame.hero;

    if (AABB(h->x, h->y, h->width, h->height,
             pillar->x, pillar->y, pillar->width, pillar->height))
    {
        if (h->x < pillar->x)
            h->x = pillar->x - h->width;
        else
            h->x = pillar->x + pillar->width;
        return;
    }
}

int CalculateTimeBonus()
{
    return (int)(CURRENT_LEVEL.timeLeft * BONUS_PER_SECOND);
}

void ResolveBalloonPillarCollision(Balloon* b, StaticObject* wall){
  if(wall->width <= 0) return;

  bool collisionX = (b->x + b->radius > wall->x) && (b->x - b->radius < wall->x + wall->width);
  bool collisionY = (b->y + b->radius > wall->y) && (b->y - b->radius < wall->y + wall->height);

  if(collisionX && collisionY){
    float penLeft   = (b->x + b->radius) - wall->x;
    float penRight  = (wall->x + wall->width) - (b->x - b->radius);
    float penTop    = (b->y + b->radius) - wall->y;
    float penBottom = (wall->y + wall->height) - (b->y - b->radius);

    float minPen = std::min({penLeft, penRight, penTop, penBottom});

    if (minPen == penLeft) {
            b->x = wall->x - b->radius;
            b->speedX = -b->speedX;
        }
        else if (minPen == penRight) {
            b->x = wall->x + wall->width + b->radius;
            b->speedX = -b->speedX;
        }
        else if (minPen == penTop) {
            b->y = wall->y - b->radius;
            b->speedY = -b->bounceSpeed;
        }
        else if (minPen == penBottom) {
            b->y = wall->y + wall->height + b->radius;
            b->speedY = fabs(b->speedY);
        }
    }
}


void CheckCollisions(){
    float harpoonCenterX = gGame.harpoon.x + gGame.harpoon.width / 2.0f;
    float harpoonTop = gGame.harpoon.y - gGame.harpoon.length;
    float harpoonBottom = gGame.harpoon.y;

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (!CURRENT_LEVEL.balloons[i].active) continue;

        Balloon* b = &CURRENT_LEVEL.balloons[i];

        // ========== CHECK COLLISION WITH PLAYER 1 ==========
        bool p1Alive = !gGame.gameState.isMultiplayer || gGame.player1Stats.lives > 0;

        float closestX = std::max(float(gGame.hero.x + 5), std::min(b->x, (float)(gGame.hero.x + gGame.hero.width - 5)));
        float closestY = std::max((float)gGame.hero.y, std::min(b->y,(float)(gGame.hero.y + gGame.hero.height)));
        float dx = b->x - closestX;
        float dy = b->y - closestY;
        float distance = sqrt(dx*dx + dy*dy);

        if(p1Alive && distance < b->radius && gGame.hero.heroHitCooldown <= 0){
            if(gGame.settingsState.soundState.soundEffectsOn)
                PlaySound(MAKEINTRESOURCE(IDR_DAMAGE_SOUND), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);

            if(gGame.gameState.isMultiplayer){
                --gGame.player1Stats.lives;
                if(gGame.player1Stats.lives <= 0 && gGame.player2Stats.lives <= 0) {
                    gGame.gameState.isGameOver = true;
                    if(!gGame.gameState.isScoreSaved) {
                        SaveFinalScore(); // Pretpostavka: Imas ovu funkciju negdje definisanu ili je to SaveScore
                        gGame.gameState.isScoreSaved = true;
                    }
                    gGame.gameState.currentMode = GAME_OVER;
                }
            } else {
                --gGame.player1Stats.lives;
                if(gGame.player1Stats.lives == 0) {
                    gGame.gameState.isGameOver = true;
                     if(!gGame.gameState.isScoreSaved) {
                        SaveFinalScore();
                        gGame.gameState.isScoreSaved = true;
                    }
                    gGame.gameState.currentMode = GAME_OVER;
                }
            }

            gGame.hero.heroHitCooldown = HERO_INVINCIBLE_TIME;
            gGame.hero2.blinkTimer = 0.0f;
            break;
        }

        // ========== CHECK COLLISION WITH HARPOON 1 ==========
        if(gGame.harpoon.isActive){
            float distanceHarpoonX = fabs(b->x - harpoonCenterX);

            if (distanceHarpoonX <= b->radius) {
                if (harpoonTop <= b->y + b->radius && harpoonBottom >= b->y - b->radius) {
                    SplitBalloon(i, gGame.harpoon.ownerPlayer);

                    // PROVJERA KRAJA LEVELA
                    if(CURRENT_LEVEL.activeBalloonCount == 0 && !gGame.gameState.isLevelCleared){

                        // 1. Izračunaj preostale živote za zvjezdice
                        int livesLeft = gGame.gameState.isMultiplayer
                            ? std::max(gGame.player1Stats.lives, gGame.player2Stats.lives)
                            : gGame.player1Stats.lives;

                        int earned = (livesLeft >= 3) ? 3 : (livesLeft == 2) ? 2 : 1;

                        // 2. NOVA LOGIKA ZA ZVJEZDICE
                        if (gGame.gameState.isMultiplayer) {
                            if (earned > gGame.levelStarsMulti[gGame.currentLevel]) {
                                gGame.levelStarsMulti[gGame.currentLevel] = earned;
                                SaveLevelStars(gGame.playerName, "multi", gGame.currentLevel, earned);
                            }
                        } else {
                            if (earned > gGame.levelStarsSingle[gGame.currentLevel]) {
                                gGame.levelStarsSingle[gGame.currentLevel] = earned;
                                SaveLevelStars(gGame.playerName, "single", gGame.currentLevel, earned);
                            }
                        }

                        gGame.gameState.isLevelCleared = true;
                        int timeBonus = CalculateTimeBonus();
                        CURRENT_LEVEL.levelScore += timeBonus;

                        if (gGame.gameState.isMultiplayer) {
                            int bonusPerPlayer = timeBonus / 2;
                            gGame.player1Stats.score += bonusPerPlayer;
                            gGame.player2Stats.score += bonusPerPlayer;
                            gGame.totalScore = gGame.player1Stats.score + gGame.player2Stats.score;

                            // SPAŠAVANJE OTKLJUČANIH NIVOA (PROGRESS)
                            if (gGame.currentLevel + 1 > gGame.unlockedLevelMulti) {
                                gGame.unlockedLevelMulti = gGame.currentLevel + 1;
                                if (gGame.unlockedLevelMulti > 6) gGame.unlockedLevelMulti = 6;
                                SavePlayerProgress(gGame.playerName, "2 PLAYERS", gGame.unlockedLevelMulti);
                            }
                        } else {
                            gGame.totalScore += timeBonus;

                            // SPAŠAVANJE OTKLJUČANIH NIVOA (PROGRESS)
                            if (gGame.currentLevel + 1 > gGame.unlockedLevelSingle) {
                                gGame.unlockedLevelSingle = gGame.currentLevel + 1;
                                if (gGame.unlockedLevelSingle > 6) gGame.unlockedLevelSingle = 6;
                                SavePlayerProgress(gGame.playerName, "1 PLAYER", gGame.unlockedLevelSingle);
                            }
                        }
                    }
                    gGame.harpoon.isActive = false;
                    break;
                }
            }
        }
    }

    // ========== MULTIPLAYER - PLAYER 2 COLLISIONS ==========
    if(!gGame.gameState.isMultiplayer) return;

    float harpoon2CenterX = gGame.harpoon2.x + gGame.harpoon2.width / 2.0f;
    float harpoon2Top = gGame.harpoon2.y - gGame.harpoon2.length;
    float harpoon2Bottom = gGame.harpoon2.y;

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (!CURRENT_LEVEL.balloons[i].active) continue;

        Balloon* b = &CURRENT_LEVEL.balloons[i];

        // ========== CHECK COLLISION WITH PLAYER 2 ==========
        if (gGame.player2Stats.lives > 0) {
            float closestX = std::max(float(gGame.hero2.x + 5),
                                     std::min(b->x, (float)(gGame.hero2.x + gGame.hero2.width - 5)));
            float closestY = std::max((float)gGame.hero2.y,
                                     std::min(b->y,(float)(gGame.hero2.y + gGame.hero2.height)));
            float dx = b->x - closestX;
            float dy = b->y - closestY;
            float distance = sqrt(dx*dx + dy*dy);

            if(distance < b->radius && gGame.hero2.heroHitCooldown <= 0) {
                if(gGame.settingsState.soundState.soundEffectsOn)
                    PlaySound(MAKEINTRESOURCE(IDR_DAMAGE_SOUND), GetModuleHandle(NULL),
                             SND_RESOURCE | SND_ASYNC);

                --gGame.player2Stats.lives;

                if(gGame.player2Stats.lives <= 0) {
                    gGame.harpoon2.isActive = false;
                }

                if(gGame.player1Stats.lives <= 0 && gGame.player2Stats.lives <= 0) {
                    gGame.gameState.isGameOver = true;
                    if(!gGame.gameState.isScoreSaved) {
                        SaveFinalScore();
                        gGame.gameState.isScoreSaved = true;
                    }
                    gGame.gameState.currentMode = GAME_OVER;
                }

                gGame.hero2.heroHitCooldown = HERO_INVINCIBLE_TIME;
                gGame.hero2.blinkTimer = 0.0f;
                break;
            }
        }

        // ========== CHECK COLLISION WITH HARPOON 2 ==========
        if(gGame.harpoon2.isActive) {
            float distanceHarpoonX = fabs(b->x - harpoon2CenterX);

            if (distanceHarpoonX <= b->radius) {
                if (harpoon2Top <= b->y + b->radius && harpoon2Bottom >= b->y - b->radius) {
                    SplitBalloon(i, gGame.harpoon2.ownerPlayer);

                    if(CURRENT_LEVEL.activeBalloonCount == 0 && !gGame.gameState.isLevelCleared) {

                         // 1. Izračunaj živote
                         int livesLeft = std::max(gGame.player1Stats.lives, gGame.player2Stats.lives);
                         int earned = (livesLeft >= 3) ? 3 : (livesLeft == 2) ? 2 : 1;

                         // 2. NOVA LOGIKA ZA ZVJEZDICE (Ovdje smo sigurno u multiplayeru jer harpun 2 postoji)
                         if (earned > gGame.levelStarsMulti[gGame.currentLevel]) {
                             gGame.levelStarsMulti[gGame.currentLevel] = earned;
                             SaveLevelStars(gGame.playerName, "multi", gGame.currentLevel, earned);
                         }

                        gGame.gameState.isLevelCleared = true;
                        int timeBonus = CalculateTimeBonus();
                        CURRENT_LEVEL.levelScore += timeBonus;

                        int bonusPerPlayer = timeBonus / 2;
                        gGame.player1Stats.score += bonusPerPlayer;
                        gGame.player2Stats.score += bonusPerPlayer;
                        gGame.totalScore = gGame.player1Stats.score + gGame.player2Stats.score;

                        // SPAŠAVANJE PROGRESSA
                        if (gGame.currentLevel + 1 > gGame.unlockedLevelMulti) {
                            gGame.unlockedLevelMulti = gGame.currentLevel + 1;
                            if (gGame.unlockedLevelMulti > 6) gGame.unlockedLevelMulti = 6;
                            SavePlayerProgress(gGame.playerName, "2 PLAYERS", gGame.unlockedLevelMulti);
                        }
                    }
                    gGame.harpoon2.isActive = false;
                    break;
                }
            }
        }
    }
}
