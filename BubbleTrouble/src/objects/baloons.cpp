#include "resources.h"
#include "gameContext.h"
#include "game.h"
#include "constants.h"
#include <windows.h>
#include <algorithm>
#include <cmath>


int CalculateTimeBonus()
{
    return (int)(CURRENT_LEVEL.timeLeft * BONUS_PER_SECOND);
}


int GetScoreForBalloon(float radius)
{
    if (radius >= 40.0f) return 10;
    if (radius >= 25.0f) return 20;
    return 40;
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



void UpdateBalloons(HWND hwnd) {
    if(gGame.gameState.isGameOver)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    int FLOOR_Y = rect.bottom - gGame.floorWall.height;
    int LEFT_WALL_X = gGame.leftWall.width;
    int RIGHT_WALL_X = rect.right - gGame.rightWall.width;

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (!CURRENT_LEVEL.balloons[i].active) continue;

        Balloon* b = &CURRENT_LEVEL.balloons[i];

        //gravitacija
        b->speedY += BALLOON_GRAVITY;
        if(b->speedY > MAX_FALL_SPEED)
            b->speedY = MAX_FALL_SPEED;

        //pomicanje
        b->x += b->speedX;
        b->y += b->speedY;

        //lijevi zid check
        if (b->x - b->radius <= LEFT_WALL_X) {
            b->x = LEFT_WALL_X + b->radius;
            b->speedX = -b->speedX;
        }

        //desni zid check
        if (b->x + b->radius >= RIGHT_WALL_X) {
            b->x = RIGHT_WALL_X - b->radius;
            b->speedX = -b->speedX;
        }

        //pod check
        if (b->y + b->radius >= FLOOR_Y) {
            b->y = FLOOR_Y - b->radius;
            b->speedY = -b->bounceSpeed;
        }

        //plafon check
        if (b->y - b->radius <= 0) {
            b->y = b->radius;
            b->speedY = 0.0f;
        }

        // ===== PREPREKE UNUTAR NIVOA ===== //
        ResolveBalloonPillarCollision(b, &CURRENT_LEVEL.longWall);
        if(CURRENT_LEVEL.door.active)
            ResolveBalloonPillarCollision(b, &CURRENT_LEVEL.door);

        /*
        AKO DODAMO VISE ZIDOVA ONDA CEMO IMATI NESTO OVAKO :
        for(int j = 0; j < CURRENT_LEVEL.numExtraWalls; j++) {
             ResolveBalloonPillarCollision(b, &CURRENT_LEVEL.extraWalls[j]);
        }
        */

        float minBounce = gGame.hero.height + 40.0f;
        float estimatedPeak  = b->bounceSpeed * 12.0f;
        /*if(estimatedPeak < minBounce)
            b->bounceSpeed +=0.2f;
          */

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
        float closestX = std::max(float(gGame.hero.x), std::min(b->x, (float)(gGame.hero.x + gGame.hero.width)));
        float closestY = std::max((float)gGame.hero.y, std::min(b->y,(float)(gGame.hero.y + gGame.hero.height)));
        float dx = b->x - closestX;
        float dy = b->y - closestY;
        float distance = sqrt(dx*dx + dy*dy);
        if(distance < b->radius && gGame.hero.heroHitCooldown <= 0) {
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

void SplitBalloon(int index) {
    Balloon* b = &CURRENT_LEVEL.balloons[index];

    int gainedScore = GetScoreForBalloon(b->radius);
    CURRENT_LEVEL.levelScore += gainedScore;
    gGame.totalScore += gainedScore;


    float newRadius = b->radius /2.0f;

    PlaySound(MAKEINTRESOURCE(IDR_BALLOON_POP),
                                  GetModuleHandle(NULL),
                                  SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);



    if (newRadius >= MIN_RADIUS) {
        for (int i = 0; i < MAX_BALLOONS; i++) {
            if (!CURRENT_LEVEL.balloons[i].active) {
                InitBalloon(i, b->x - 10, b->y, newRadius, -3.5f, b->color);
                CURRENT_LEVEL.balloons[i].speedY = -CURRENT_LEVEL.balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;
                break;
            }
        }

        for (int i = 0; i < MAX_BALLOONS; i++) {
            if (!CURRENT_LEVEL.balloons[i].active) {
                InitBalloon(i, b->x + 10, b->y, newRadius, 3.5f, b->color);
                CURRENT_LEVEL.balloons[i].speedY = -CURRENT_LEVEL.balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;

                break;
            }
        }
    }

    b->active = false;
    CURRENT_LEVEL.activeBalloonCount--;

}

void DrawBalloonGDI(HDC hdc, Balloon* b) {
    int centerX = (int)b->x;
    int centerY = (int)b->y;
    int radius = (int)b->radius;

    int r = GetRValue(b->color);
    int g = GetGValue(b->color);
    int b_val = GetBValue(b->color);

    for (int i = radius; i > 0; i--) {
        float factor = (float)i / radius;
        int newR = r + (int)((255 - r) * (1.0f - factor) * 0.4f);
        int newG = g + (int)((255 - g) * (1.0f - factor) * 0.4f);
        int newB = b_val + (int)((255 - b_val) * (1.0f - factor) * 0.4f);

        HBRUSH brush = CreateSolidBrush(RGB(newR, newG, newB));
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(newR, newG, newB));

        SelectObject(hdc, brush);
        SelectObject(hdc, pen);

        Ellipse(hdc, centerX - i, centerY - i, centerX + i, centerY + i);

        DeleteObject(brush);
        DeleteObject(pen);
    }

    HPEN outlinePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    SelectObject(hdc, outlinePen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Ellipse(hdc, centerX - radius, centerY - radius,
            centerX + radius, centerY + radius);
    DeleteObject(outlinePen);

    HBRUSH highlightBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, highlightBrush);
    SelectObject(hdc, GetStockObject(NULL_PEN));

    int hlX = centerX - radius / 3;
    int hlY = centerY - radius / 2;
    int hlW = radius / 2;
    int hlH = radius / 3;
    Ellipse(hdc, hlX - hlW/2, hlY - hlH/2, hlX + hlW/2, hlY + hlH/2);

    DeleteObject(highlightBrush);
}
