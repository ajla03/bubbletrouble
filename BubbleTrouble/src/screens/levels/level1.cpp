#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"

void InitLevel1(){
    InitBalloon(1, 400, 150, 40, -2.0f, RGB	(255,141,0));
    InitPowerupSystemForLevel();
}

void RenderLevel1(HDC hdcBuffer, RECT rect)
{
    SelectObject(gRes.hdcMem, gRes.background);

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    StretchBlt(
        hdcBuffer, bgX, bgY, bgW, bgH,
        gRes.hdcMem, 0, 0,
        CURRENT_LEVEL.backgroundInfo.width,
        CURRENT_LEVEL.backgroundInfo.height,
        SRCCOPY
    );

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }

    DrawPowerups(hdcBuffer, rect);
}


