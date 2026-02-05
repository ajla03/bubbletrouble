#include "resources.h"
#include "gameContext.h"
#include "game.h"
#include "constants.h"
#include <windows.h>
#include <algorithm>
#include <cmath>

// Moved from resources.cpp
float GetBounceSpeedForRadius(float radius)
{
    float t = radius / MAX_RADIUS;
    return MIN_BOUNCE_SPEED + t * (MAX_BOUNCE_SPEED - MIN_BOUNCE_SPEED);
}

// Moved from resources.cpp
void InitBalloon(int index, float x, float y, float radius, float speedX, COLORREF color)
{
    if (index >= MAX_BALLOONS) return;

    CURRENT_LEVEL.balloons[index].x = x;
    CURRENT_LEVEL.balloons[index].y = y;
    CURRENT_LEVEL.balloons[index].radius = radius;
    CURRENT_LEVEL.balloons[index].speedX = speedX * 0.85f;
    CURRENT_LEVEL.balloons[index].speedY = 0.0f;
    CURRENT_LEVEL.balloons[index].bounceSpeed = GetBounceSpeedForRadius(radius);
    CURRENT_LEVEL.balloons[index].active = true;
    CURRENT_LEVEL.balloons[index].color = color;
    CURRENT_LEVEL.activeBalloonCount++;
    CURRENT_LEVEL.balloons[index].isFrozen = false;
    CURRENT_LEVEL.balloons[index].frozenSpeedX = 0.0f;
    CURRENT_LEVEL.balloons[index].frozenSpeedY = 0.0f;
}

// Moved from update.cpp
bool AreSectionBalloonsDestroyed(float left, float right)
{
    for (int i = 0; i < MAX_BALLOONS; i++)
    {
        Balloon* b = &CURRENT_LEVEL.balloons[i];
        if (!b->active) continue;

        if (b->x > left && b->x < right)
            return false;
    }
    return true;
}


int GetScoreForBalloon(float radius)
{
    if (radius >= 40.0f) return 10;
    if (radius >= 25.0f) return 20;
    return 40;
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
        // Preskoči fiziku ako je balon frozen
        if(b->isFrozen) {
            continue;  // Ne pomjeraj balon
        }

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

        // Level 4 pillars
        ResolveBalloonPillarCollision(b, &CURRENT_LEVEL.pillar1);
        ResolveBalloonPillarCollision(b, &CURRENT_LEVEL.pillar2);

        float minBounce = gGame.hero.height + 40.0f;
        float estimatedPeak  = b->bounceSpeed * 12.0f;
        if(estimatedPeak < minBounce)
            b->bounceSpeed +=0.2f;
    }
}


void SplitBalloon(int index, int scoringPlayer) {  // ← DODAJ PARAMETAR
    Balloon* b = &CURRENT_LEVEL.balloons[index];

    int gainedScore = GetScoreForBalloon(b->radius);
    CURRENT_LEVEL.levelScore += gainedScore;

    // ← NOVI KOD - ODVOJENI SCORE
    if(gGame.gameState.isMultiplayer) {
        if(scoringPlayer == 1) {
            gGame.player1Stats.score += gainedScore;
        } else if(scoringPlayer == 2) {
            gGame.player2Stats.score += gainedScore;
        }
        gGame.totalScore = gGame.player1Stats.score + gGame.player2Stats.score;
    } else {
        gGame.totalScore += gainedScore;
    }

    float newRadius = b->radius / 2.0f;

    if(gGame.settingsState.soundState.soundEffectsOn)
        PlaySound(MAKEINTRESOURCE(IDR_BALLOON_POP),
                                  GetModuleHandle(NULL),
                                  SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);

    // Zapamti frozen stanje roditeljskog balona
    bool parentWasFrozen = b->isFrozen;

    if (newRadius >= MIN_RADIUS) {
        // Prvi novi balon (lijevo)
        for (int i = 0; i < MAX_BALLOONS; i++) {
            if (!CURRENT_LEVEL.balloons[i].active) {
                InitBalloon(i, b->x - 10, b->y, newRadius, -3.5f, b->color);

                if (parentWasFrozen) {
                    CURRENT_LEVEL.balloons[i].isFrozen = true;
                    CURRENT_LEVEL.balloons[i].frozenSpeedX = -3.5f * 0.85f;
                    CURRENT_LEVEL.balloons[i].frozenSpeedY = -CURRENT_LEVEL.balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;
                    CURRENT_LEVEL.balloons[i].speedX = 0.0f;
                    CURRENT_LEVEL.balloons[i].speedY = 0.0f;
                } else {
                    CURRENT_LEVEL.balloons[i].speedY = -CURRENT_LEVEL.balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;
                }
                break;
            }
        }

        // Drugi novi balon (desno)
        for (int i = 0; i < MAX_BALLOONS; i++) {
            if (!CURRENT_LEVEL.balloons[i].active) {
                InitBalloon(i, b->x + 10, b->y, newRadius, 3.5f, b->color);

                if (parentWasFrozen) {
                    CURRENT_LEVEL.balloons[i].isFrozen = true;
                    CURRENT_LEVEL.balloons[i].frozenSpeedX = 3.5f * 0.85f;
                    CURRENT_LEVEL.balloons[i].frozenSpeedY = -CURRENT_LEVEL.balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;
                    CURRENT_LEVEL.balloons[i].speedX = 0.0f;
                    CURRENT_LEVEL.balloons[i].speedY = 0.0f;
                } else {
                    CURRENT_LEVEL.balloons[i].speedY = -CURRENT_LEVEL.balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;
                }
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
    int blue_val = GetBValue(b->color);  // ← PROMIJENJEN NAZIV (bio b_val)

    for (int i = radius; i > 0; i--) {
        float factor = (float)i / radius;
        int newR = r + (int)((255 - r) * (1.0f - factor) * 0.4f);
        int newG = g + (int)((255 - g) * (1.0f - factor) * 0.4f);
        int newB = blue_val + (int)((255 - blue_val) * (1.0f - factor) * 0.4f);

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
