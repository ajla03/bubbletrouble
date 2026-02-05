#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>


void RenderLevel3(HDC hdcBuffer, RECT rect){

    // === BACKGROUND ===
    SelectObject(gRes.hdcMem, gRes.hBgLevel3);

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

    // === DOOR ===
    if(CURRENT_LEVEL.door.active){
        SelectObject(gRes.hdcMem, gRes.doorClosed);
        StretchBlt(hdcBuffer,
            CURRENT_LEVEL.door.x,
            CURRENT_LEVEL.door.y,
            CURRENT_LEVEL.door.width,
            CURRENT_LEVEL.door.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.door.originalWidth,
            CURRENT_LEVEL.door.originalHeight,
            SRCAND);

        SelectObject(gRes.hdcMem, gRes.doorClosedMask);
        StretchBlt(hdcBuffer,
            CURRENT_LEVEL.door.x,
            CURRENT_LEVEL.door.y,
            CURRENT_LEVEL.door.width,
            CURRENT_LEVEL.door.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.door.originalWidth,
            CURRENT_LEVEL.door.originalHeight,
            SRCPAINT);
    }else{
        SelectObject(gRes.hdcMem, gRes.door);
        StretchBlt(hdcBuffer,
            CURRENT_LEVEL.door.x,
            CURRENT_LEVEL.door.y,
            CURRENT_LEVEL.door.width,
            CURRENT_LEVEL.door.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.door.originalWidth,
            CURRENT_LEVEL.door.originalHeight,
            SRCAND);

        SelectObject(gRes.hdcMem, gRes.doorMask);
        StretchBlt(hdcBuffer,
            CURRENT_LEVEL.door.x,
            CURRENT_LEVEL.door.y,
            CURRENT_LEVEL.door.width,
            CURRENT_LEVEL.door.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.door.originalWidth,
            CURRENT_LEVEL.door.originalHeight,
            SRCPAINT);
    }
    // === PILLAR ===
    SelectObject(gRes.hdcMem, gRes.longWall);
    SetStretchBltMode(hdcBuffer, HALFTONE);
    SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
    StretchBlt(
        hdcBuffer,
        CURRENT_LEVEL.longWall.x,
        CURRENT_LEVEL.longWall.y,
        CURRENT_LEVEL.longWall.width,
        CURRENT_LEVEL.longWall.height,
        gRes.hdcMem,
        0, 0,
        CURRENT_LEVEL.longWall.originalWidth,
        CURRENT_LEVEL.longWall.originalHeight,
        SRCCOPY
    );

    // === BALLOONS ===
    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }

    DrawPowerups(hdcBuffer, rect);

}

void InitLevel3(HWND hwnd){
  RECT rect;
  GetClientRect(hwnd, &rect);

  if(gRes.hBgLevel3){
    BITMAP bm;
    GetObject(gRes.hBgLevel2, sizeof(BITMAP), &bm);
    CURRENT_LEVEL.backgroundInfo.width = bm.bmWidth;
    CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
  }

  BITMAP bmp;
  GetObject(gRes.longWall, sizeof(BITMAP), &bmp);
  CURRENT_LEVEL.longWall.originalWidth = bmp.bmWidth;
  CURRENT_LEVEL.longWall.originalHeight= bmp.bmHeight;
  CURRENT_LEVEL.longWall.width = bmp.bmWidth;
  CURRENT_LEVEL.longWall.height= bmp.bmHeight;

  GetObject(gRes.doorClosed, sizeof(BITMAP), &bmp);
  CURRENT_LEVEL.door.originalWidth = bmp.bmWidth;
  CURRENT_LEVEL.door.originalHeight= bmp.bmHeight;

  RecalculateLevel3Layout(hwnd);
  CURRENT_LEVEL.door.active = true;

  for(int i=0;i<MAX_BALLOONS;i++)
        CURRENT_LEVEL.balloons[i].active = false;

  InitBalloon(0, rect.right/4, 100, 20, 3.5f, RGB(255, 0, 0));    // Crvena
  InitBalloon(1, 3*rect.right/4, 150, 40, -3.0f, RGB(0, 255, 0));   // Zelena
  //InitBalloon(2, 600, 120, 80, 3.0f, RGB(0, 100, 255));  // Plava
  InitPowerupSystemForLevel();
  if (gGame.gameState.isMultiplayer) {
      gGame.hero.x = gGame.leftWall.width + 50;
      gGame.hero2.x = gGame.leftWall.width + 130;

      gGame.hero2.y = rect.bottom - gGame.floorWall.height - gGame.hero2.height;
      gGame.hero2.dx = gGame.hero.dx;
  }
}


void RecalculateLevel3Layout(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    int bgH = rect.bottom - gGame.floorWall.height;
    int targetDoorW = CURRENT_LEVEL.longWall.width;
    int targetDoorH = gGame.hero.height + 5;

    // Door
    CURRENT_LEVEL.door.x = rect.right/2 - targetDoorW/2;
    CURRENT_LEVEL.door.y = bgH - targetDoorH;
    CURRENT_LEVEL.door.width = targetDoorW;
    CURRENT_LEVEL.door.height = targetDoorH;

    // Pillar
    CURRENT_LEVEL.longWall.x = rect.right/2 - CURRENT_LEVEL.longWall.width/2;
    CURRENT_LEVEL.longWall.y = 0;
    CURRENT_LEVEL.longWall.height = CURRENT_LEVEL.door.y;
}

