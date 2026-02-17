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

        // Padanje
        p->y += p->speedY;

        // Koordinate powerupa (pomaknuto iznad provjere za pod)
        float powerupLeft = p->x;
        float powerupRight = p->x + p->width;
        float powerupTop = p->y;
        float powerupBottom = p->y + p->height;

        // === PROVJERA SUDARA SA PREPREKAMA ===
        bool hitObstacle = false;

        // 1. Dugacki zid
        if (CURRENT_LEVEL.longWall.width > 0 &&
            powerupBottom > CURRENT_LEVEL.longWall.y && powerupTop < CURRENT_LEVEL.longWall.y + CURRENT_LEVEL.longWall.height &&
            powerupRight > CURRENT_LEVEL.longWall.x && powerupLeft < CURRENT_LEVEL.longWall.x + CURRENT_LEVEL.longWall.width) {
            hitObstacle = true;
        }

        // 2. Vrata
        if (CURRENT_LEVEL.ladder.width == 0 && CURRENT_LEVEL.door.active && CURRENT_LEVEL.door.width > 0 &&
            powerupBottom > CURRENT_LEVEL.door.y && powerupTop < CURRENT_LEVEL.door.y + CURRENT_LEVEL.door.height &&
            powerupRight > CURRENT_LEVEL.door.x && powerupLeft < CURRENT_LEVEL.door.x + CURRENT_LEVEL.door.width) {
            hitObstacle = true;
        }

        // 3. Stub 1 (Level 4)
        if (CURRENT_LEVEL.pillar1.width > 0 &&
            powerupBottom > CURRENT_LEVEL.pillar1.y && powerupTop < CURRENT_LEVEL.pillar1.y + CURRENT_LEVEL.pillar1.height &&
            powerupRight > CURRENT_LEVEL.pillar1.x && powerupLeft < CURRENT_LEVEL.pillar1.x + CURRENT_LEVEL.pillar1.width) {
            hitObstacle = true;
        }

        // 4. Stub 2 (Level 4)
        if (CURRENT_LEVEL.pillar2.width > 0 &&
            powerupBottom > CURRENT_LEVEL.pillar2.y && powerupTop < CURRENT_LEVEL.pillar2.y + CURRENT_LEVEL.pillar2.height &&
            powerupRight > CURRENT_LEVEL.pillar2.x && powerupLeft < CURRENT_LEVEL.pillar2.x + CURRENT_LEVEL.pillar2.width) {
            hitObstacle = true;
        }

        // Ukloni ako padne na pod ILI na neku od prepreka
        if(powerupBottom >= FLOOR_Y || hitObstacle) {
            p->active = false;
            CURRENT_LEVEL.activePowerupCount--;
            continue;
        }

        // === PROVJERA SUDARA SA PLAYER 1 ===
        float heroLeft = gGame.hero.x;
        float heroRight = gGame.hero.x + gGame.hero.width;
        float heroTop = gGame.hero.y;
        float heroBottom = gGame.hero.y + gGame.hero.height;

        if(heroRight > powerupLeft && heroLeft < powerupRight &&
           heroBottom > powerupTop && heroTop < powerupBottom && gGame.player1Stats.lives > 0) {

            ApplyPowerup(p->type, 1); // <--- ID 1 za Player 1
            p->active = false;
            CURRENT_LEVEL.activePowerupCount--;

            if(gGame.settingsState.soundState.soundEffectsOn)
                PlaySound(MAKEINTRESOURCE(IDR_BALLOON_POP),
                     GetModuleHandle(NULL),
                     SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
            continue; // Pređi na sljedeći powerup
        }

        // === PROVJERA SUDARA SA PLAYER 2 (Samo Multiplayer) ===
        if (gGame.gameState.isMultiplayer) {
            float hero2Left = gGame.hero2.x;
            float hero2Right = gGame.hero2.x + gGame.hero2.width;
            float hero2Top = gGame.hero2.y;
            float hero2Bottom = gGame.hero2.y + gGame.hero2.height;

            if (hero2Right > powerupLeft && hero2Left < powerupRight &&
                hero2Bottom > powerupTop && hero2Top < powerupBottom && gGame.player2Stats.lives > 0) {

                ApplyPowerup(p->type, 2); // <--- ID 2 za Player 2
                p->active = false;
                CURRENT_LEVEL.activePowerupCount--;

                if (gGame.settingsState.soundState.soundEffectsOn)
                     PlaySound(MAKEINTRESOURCE(IDR_BALLOON_POP),
                         GetModuleHandle(NULL),
                         SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
                continue;
            }
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

void ApplyPowerup(PowerUpType type, int playerIndex) {
    switch(type) {
        case POWERUP_EXTRA_LIFE:
            // Logika za živote ovisi o modu igre i igraču
            if (gGame.gameState.isMultiplayer) {
                if (playerIndex == 1) {
                    if (gGame.player1Stats.lives < 5)
                        gGame.player1Stats.lives++;
                }
                else if (playerIndex == 2) {
                    if (gGame.player2Stats.lives < 5)
                        gGame.player2Stats.lives++;
                }
            } else {
                // Single player standardno
                if (gGame.player1Stats.lives < 5)
                    gGame.player1Stats.lives++;
            }
            break;

        case POWERUP_EXTRA_TIME:
            CURRENT_LEVEL.timeLeft += TIME_BONUS_AMOUNT;
            if(CURRENT_LEVEL.timeLeft > maxTime) {
                CURRENT_LEVEL.timeLeft = maxTime;
            }
            break;

        case POWERUP_FREEZE:
            // Zamrzni sve balone (vrijedi za oba igrača)
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

        if(!b->isFrozen) {
            b->frozenSpeedX = b->speedX;
            b->frozenSpeedY = b->speedY;

            b->speedX = 0.0f;
            b->speedY = 0.0f;

            b->isFrozen = true;
        }
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
