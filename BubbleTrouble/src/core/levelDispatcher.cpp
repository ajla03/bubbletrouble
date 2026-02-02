#include "game.h"
#include "gameContext.h"

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
    }
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
    }
}
