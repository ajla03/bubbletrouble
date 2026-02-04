#include "resources.h"
#include "gameContext.h"
#include "game.h"
#include "constants.h"
#include <windows.h>
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
    float penLeft   = (b->x + b->radius) - wall->x;                     // Udarac u LIJEVU stranu zida
    float penRight  = (wall->x + wall->width) - (b->x - b->radius);     // Udarac u DESNU stranu zida
    float penTop    = (b->y + b->radius) - wall->y;                     // Udarac u VRH zida
    float penBottom = (wall->y + wall->height) - (b->y - b->radius);    // Udarac u DNO zida

    float minPen = std::min({penLeft, penRight, penTop, penBottom});


    if (minPen == penLeft) {
            b->x = wall->x - b->radius;
            b->speedX = -b->speedX; // Forsiraj negativnu brzinu (lijevo)
        }
        else if (minPen == penRight) {
            b->x = wall->x + wall->width + b->radius;
            b->speedX = -b->speedX;  // Forsiraj pozitivnu brzinu (desno)
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
    float harpoonBottom =gGame.harpoon.y;

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (!CURRENT_LEVEL.balloons[i].active) continue;

        Balloon* b = &CURRENT_LEVEL.balloons[i];
        // check collision with hero
        float closestX = std::max(float(gGame.hero.x + 5), std::min(b->x, (float)(gGame.hero.x + gGame.hero.width - 5)));
        float closestY = std::max((float)gGame.hero.y, std::min(b->y,(float)(gGame.hero.y + gGame.hero.height)));
        float dx = b->x - closestX;
        float dy = b->y - closestY;
        float distance = sqrt(dx*dx + dy*dy);

        if(distance < b->radius && gGame.hero.heroHitCooldown <= 0){
              if(gGame.soundState.soundEffectsOn)
                PlaySound(MAKEINTRESOURCE(IDR_DAMAGE_SOUND), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
            --gGame.gameState.lives;
            gGame.hero.heroHitCooldown = HERO_INVINCIBLE_TIME;
            if(gGame.gameState.lives==0){
                gGame.gameState.isGameOver = true;
                gGame.gameState.currentMode = GAME_OVER;
            }
            break;
        }


        // check collision with harpoon
        if(gGame.harpoon.isActive){

        float distanceHarpoonX = fabs(b->x - harpoonCenterX);

        if (distanceHarpoonX <= b->radius) {
                if (harpoonTop <= b->y + b->radius && harpoonBottom >= b->y - b->radius) {
                    SplitBalloon(i);
                    if(CURRENT_LEVEL.activeBalloonCount == 0 && !gGame.gameState.isLevelCleared){
                        gGame.gameState.isLevelCleared = true;
                         int timeBonus = CalculateTimeBonus();
                         CURRENT_LEVEL.levelScore += timeBonus;
                         gGame.totalScore += timeBonus;
                    }
                    gGame.harpoon.isActive = false;
                    break;
                }
            }
        }
    }
}
