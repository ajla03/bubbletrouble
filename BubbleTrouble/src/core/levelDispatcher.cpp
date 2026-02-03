#include "game.h"
#include "gameContext.h"
#include <ctime>
#include <cstdlib>

void RenderLevel(HDC hdcBuffer, RECT rect){
  switch (gGame.currentLevel) {
    case 0:
        RenderLevel1(hdcBuffer, rect);
        break;
    case 1:
        RenderLevel2(hdcBuffer, rect);
        break;
    case 2:
        RenderLevel3(hdcBuffer, rect);
        break;
    case 3:
        RenderLevel4(hdcBuffer, rect);
        break;
    case 4:
        RenderLevel5(hdcBuffer, rect);
        break;
    case 5:
        RenderLevel6(hdcBuffer, rect);
        break;
    }
    DrawPowerups(hdcBuffer, rect);
}


void InitLevel(HWND hwnd){
  switch (gGame.currentLevel) {
    case 0:
       // level 1 nam se inicjalizuje u resources.cpp ? Da prebacimo ovdje or not ?
        break;
    case 1:
        InitLevel2(hwnd);
        break;
    case 2:
        InitLevel3(hwnd);
        break;
    case 3:
        InitLevel4(hwnd);
        break;
    case 4:
        InitLevel5(hwnd);
        break;
    case 5:
        InitLevel6(hwnd);
        break;
    }
    CURRENT_LEVEL.activePowerupCount = 0;
    CURRENT_LEVEL.lastPowerupSpawn = GetTickCount();
    srand((unsigned)time(NULL));

    CURRENT_LEVEL.nextPowerupSpawnTime = POWERUP_SPAWN_MIN_TIME +
        (rand() % (POWERUP_SPAWN_MAX_TIME - POWERUP_SPAWN_MIN_TIME));

    for(int i = 0; i < MAX_POWERUPS; i++) {
        CURRENT_LEVEL.powerups[i].active = false;
    }
}
