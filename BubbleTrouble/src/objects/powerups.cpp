#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"
#include "resources.h"
#include "constants.h"
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

// Dodaj ovu biblioteku za TransparentBlt
#pragma comment(lib, "msimg32.lib")

void InitPowerupSystem() {
    srand((unsigned)time(NULL));

    for(int lvl = 0; lvl < MAX_LEVELS; lvl++) {
        gGame.levels[lvl].activePowerupCount = 0;
        gGame.levels[lvl].lastPowerupSpawn = GetTickCount();
        gGame.levels[lvl].nextPowerupSpawnTime = POWERUP_SPAWN_MIN_TIME +
            (rand() % (POWERUP_SPAWN_MAX_TIME - POWERUP_SPAWN_MIN_TIME));

        for(int i = 0; i < MAX_POWERUPS; i++) {
            gGame.levels[lvl].powerups[i].active = false;
        }
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
            if(randomChance < TIME_POWERUP_SPAWN_CHANCE) {
                p->type = POWERUP_EXTRA_TIME;  // 30% šansa za vreme
            } else {
                p->type = POWERUP_EXTRA_LIFE;  // 70% šansa za život
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

        if(p->y >= FLOOR_Y) {
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

            PlaySound(MAKEINTRESOURCE(IDR_BALLOON_POP),
                     GetModuleHandle(NULL),
                     SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
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
            // Dodaj vreme, ali nemoj preći maxTime
            CURRENT_LEVEL.timeLeft += TIME_BONUS_AMOUNT;
            if(CURRENT_LEVEL.timeLeft > maxTime) {
                CURRENT_LEVEL.timeLeft = maxTime;
            }
            break;
    }
}

void DrawPowerups(HDC hdc, RECT rect) {
    for(int i = 0; i < MAX_POWERUPS; i++) {
        if(!CURRENT_LEVEL.powerups[i].active) continue;

        PowerUp* p = &CURRENT_LEVEL.powerups[i];

        // Odaberi odgovarajuću bitmapu za tip powerup-a
        HBITMAP bitmapToUse = NULL;

        switch(p->type) {
            case POWERUP_EXTRA_LIFE:
                bitmapToUse = gRes.lifePowerupMask;
                break;
            case POWERUP_EXTRA_TIME:
                bitmapToUse = gRes.timePowerupMask;
                break;
            default:
                bitmapToUse = gRes.lifePowerupMask; // Fallback
                break;
        }

        if(!bitmapToUse) {
            // Fallback - obojeni kvadrat zavisno od tipa
            COLORREF color = (p->type == POWERUP_EXTRA_TIME) ? RGB(0, 200, 255) : RGB(255, 0, 0);
            HBRUSH brush = CreateSolidBrush(color);
            RECT r = { (int)p->x, (int)p->y,
                       (int)(p->x + p->width), (int)(p->y + p->height) };
            FillRect(hdc, &r, brush);
            DeleteObject(brush);
            continue;
        }

        // Uzmi dimenzije originalne bitmape
        BITMAP bm;
        GetObject(bitmapToUse, sizeof(BITMAP), &bm);

        SelectObject(gRes.hdcMem, bitmapToUse);

        // TransparentBlt - automatski tretira belu kao providnu
        TransparentBlt(
            hdc,
            (int)p->x, (int)p->y,
            p->width, p->height,
            gRes.hdcMem,
            0, 0,
            bm.bmWidth, bm.bmHeight,
            RGB(255, 255, 255)  // bela = providna
        );
    }
}
