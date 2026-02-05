#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"
#include "resources.h"
#include "constants.h"
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>


void InitPowerupSystemForLevel() {
    CURRENT_LEVEL.activePowerupCount = 0;
    CURRENT_LEVEL.lastPowerupSpawn = GetTickCount();
    CURRENT_LEVEL.nextPowerupSpawnTime = POWERUP_SPAWN_MIN_TIME +
        (rand() % (POWERUP_SPAWN_MAX_TIME - POWERUP_SPAWN_MIN_TIME));

    for(int i = 0; i < MAX_POWERUPS; i++) {
        CURRENT_LEVEL.powerups[i].active = false;
    }
}

void SpawnPowerup(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    for(int i = 0; i < MAX_POWERUPS; i++) {
        if(!CURRENT_LEVEL.powerups[i].active) {
            PowerUp* p = &CURRENT_LEVEL.powerups[i];

            int minX = gGame.leftWall.width + POWERUP_SIZE;
            int maxX = rect.right - gGame.rightWall.width - POWERUP_SIZE;
            p->x = minX + (rand() % (maxX - minX));
            p->y = 0;

            p->speedY = POWERUP_FALL_SPEED;

            // === RANDOM POWERUP TIP === //
            int randomChance = rand() % 100;

            if(randomChance < LIFE_POWERUP_SPAWN_CHANCE) {
                // 0-49: Life (50%)
                p->type = POWERUP_EXTRA_LIFE;
            }
            else if(randomChance < LIFE_POWERUP_SPAWN_CHANCE + TIME_POWERUP_SPAWN_CHANCE) {
                // 50-79: Time (30%)
                p->type = POWERUP_EXTRA_TIME;
            }
            else {
                // 80-99: Freeze (20%)
                p->type = POWERUP_FREEZE;
            }

            p->active = true;
            p->width = POWERUP_SIZE;
            p->height = POWERUP_SIZE;
            p->spawnTime = GetTickCount();

            CURRENT_LEVEL.activePowerupCount++;
            break;
        }
    }

    CURRENT_LEVEL.lastPowerupSpawn = GetTickCount();
    CURRENT_LEVEL.nextPowerupSpawnTime = POWERUP_SPAWN_MIN_TIME +
        (rand() % (POWERUP_SPAWN_MAX_TIME - POWERUP_SPAWN_MIN_TIME));
}

void UpdatePowerups(HWND hwnd) {
    if(gGame.gameState.isGameOver || gGame.gameState.isLevelCleared)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    DWORD currentTime = GetTickCount();

    if(currentTime - CURRENT_LEVEL.lastPowerupSpawn >= CURRENT_LEVEL.nextPowerupSpawnTime) {
        SpawnPowerup(hwnd);
    }

    int FLOOR_Y = rect.bottom - gGame.floorWall.height;

    for(int i = 0; i < MAX_POWERUPS; i++) {
        if(!CURRENT_LEVEL.powerups[i].active) continue;

        PowerUp* p = &CURRENT_LEVEL.powerups[i];

        p->y += p->speedY;

        if(p->y + p->height >= FLOOR_Y) {
            p->active = false;
            CURRENT_LEVEL.activePowerupCount--;
            continue;
        }

        float heroLeft = gGame.hero.x;
        float heroRight = gGame.hero.x + gGame.hero.width;
        float heroTop = gGame.hero.y;
        float heroBottom = gGame.hero.y + gGame.hero.height;

        float powerupLeft = p->x;
        float powerupRight = p->x + p->width;
        float powerupTop = p->y;
        float powerupBottom = p->y + p->height;

        if(heroRight > powerupLeft && heroLeft < powerupRight &&
           heroBottom > powerupTop && heroTop < powerupBottom) {

            ApplyPowerup(p->type);
            p->active = false;
            CURRENT_LEVEL.activePowerupCount--;

            if(gGame.settingsState.soundState.soundEffectsOn)
                PlaySound(MAKEINTRESOURCE(IDR_BALLOON_POP),
                     GetModuleHandle(NULL),
                     SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
        }
    }

    // === UPDATE FREEZE TIMER === //
    if(gGame.gameState.balloonsAreFrozen && gGame.gameState.freezeTimeLeft > 0) {
        gGame.gameState.freezeTimeLeft--;

        if(gGame.gameState.freezeTimeLeft <= 0) {
            // Freeze istekao - unfreezeuj balone
            UnfreezeBalloons();
        }
    }
}

void ApplyPowerup(PowerUpType type) {
    switch(type) {
        case POWERUP_EXTRA_LIFE:
            if(gGame.gameState.lives < MAX_LIVES) {
                gGame.gameState.lives++;
            }
            break;

        case POWERUP_EXTRA_TIME:
            CURRENT_LEVEL.timeLeft += TIME_BONUS_AMOUNT;
            if(CURRENT_LEVEL.timeLeft > maxTime) {
                CURRENT_LEVEL.timeLeft = maxTime;
            }
            break;

        case POWERUP_FREEZE:
            // Zamrzni sve balone
            FreezeBalloons();
            gGame.gameState.balloonsAreFrozen = true;
            gGame.gameState.freezeTimeLeft = FREEZE_DURATION;
            break;
    }
}

void FreezeBalloons() {
    for(int i = 0; i < MAX_BALLOONS; i++) {
        if(!CURRENT_LEVEL.balloons[i].active) continue;

        Balloon* b = &CURRENT_LEVEL.balloons[i];

        b->frozenSpeedX = b->speedX;
        b->frozenSpeedY = b->speedY;

        b->speedX = 0.0f;
        b->speedY = 0.0f;

        b->isFrozen = true;
    }
}

void UnfreezeBalloons() {
    gGame.gameState.balloonsAreFrozen = false;

    for(int i = 0; i < MAX_BALLOONS; i++) {
        if(!CURRENT_LEVEL.balloons[i].active) continue;

        Balloon* b = &CURRENT_LEVEL.balloons[i];

        if(b->isFrozen) {
            // Vrati originalne brzine
            b->speedX = b->frozenSpeedX;
            b->speedY = b->frozenSpeedY;

            // Ukloni frozen oznaku
            b->isFrozen = false;
        }
    }
}

void DrawPowerups(HDC hdc, RECT rect) {
    for(int i = 0; i < MAX_POWERUPS; i++) {
        if(!CURRENT_LEVEL.powerups[i].active) continue;

        PowerUp* p = &CURRENT_LEVEL.powerups[i];

        HBITMAP bitmapToUse = NULL;

        switch(p->type) {
            case POWERUP_EXTRA_LIFE:
                bitmapToUse = gRes.lifePowerupMask;
                break;
            case POWERUP_EXTRA_TIME:
                bitmapToUse = gRes.timePowerupMask;
                break;
            case POWERUP_FREEZE:
                bitmapToUse = gRes.freezePowerupMask;
                break;
            default:
                bitmapToUse = gRes.lifePowerupMask;
                break;
        }

        if(!bitmapToUse) {
            // Fallback - obojeni kvadrat
            COLORREF color;
            switch(p->type) {
                case POWERUP_EXTRA_TIME:
                    color = RGB(0, 200, 255); // Plava
                    break;
                case POWERUP_FREEZE:
                    color = RGB(150, 220, 255); // Svetlo plava/led
                    break;
                default:
                    color = RGB(255, 0, 0); // Crvena
                    break;
            }

            HBRUSH brush = CreateSolidBrush(color);
            RECT r = { (int)p->x, (int)p->y,
                       (int)(p->x + p->width), (int)(p->y + p->height) };
            FillRect(hdc, &r, brush);
            DeleteObject(brush);
            continue;
        }

        BITMAP bm;
        GetObject(bitmapToUse, sizeof(BITMAP), &bm);

        SelectObject(gRes.hdcMem, bitmapToUse);

        TransparentBlt(
            hdc,
            (int)p->x, (int)p->y,
            p->width, p->height,
            gRes.hdcMem,
            0, 0,
            bm.bmWidth, bm.bmHeight,
            RGB(255, 255, 255)
        );
    }
}
