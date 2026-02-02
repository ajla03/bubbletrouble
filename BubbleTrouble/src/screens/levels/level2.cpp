#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

void RenderLevel2(HDC hdcBuffer, RECT rect){
    // === BACKGROUND ===
    SelectObject(gRes.hdcMem, gRes.hBgLevel2);

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
    StretchBlt(
        hdcBuffer, bgX, bgY, bgW, bgH,
        gRes.hdcMem, 0, 0,
        CURRENT_LEVEL.backgroundInfo.width,
        CURRENT_LEVEL.backgroundInfo.height,
        SRCCOPY
    );

    // === BALLOONS ===
    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }
}

void InitLevel2(HWND hwnd){

  if(gRes.hBgLevel2){
    BITMAP bm;
    GetObject(gRes.hBgLevel2, sizeof(BITMAP), &bm);
    CURRENT_LEVEL.backgroundInfo.width = bm.bmWidth;
    CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
  }

  InitBalloon(0, 200, 100, 20, 3.0f, RGB(255, 0, 0));    // Crvena
  InitBalloon(1, 400, 150, 40, -2.5f, RGB(0, 255, 0));   // Zelena
  //InitBalloon(2, 600, 120, 80, 3.0f, RGB(0, 100, 255));  // Plava

}

