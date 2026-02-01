#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"

void RenderLevel1(HDC hdcBuffer, RECT rect)
{
    HDC hdcMem = CreateCompatibleDC(hdcBuffer);

    // === BACKGROUND ===
    SelectObject(hdcMem, gRes.background);

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    StretchBlt(
        hdcBuffer, bgX, bgY, bgW, bgH,
        hdcMem, 0, 0,
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

    DeleteDC(hdcMem);
}

