#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include <wingdi.h>

void RenderSettings(HDC hdcBuffer, RECT rect)
{
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);

    //  BACK BUTTON //

    BITMAP bm;
    int btnWidth = gGame.backButtonInfo.width;
    int btnHeight = gGame.backButtonInfo.height;
    int x = btnWidth/2;
    int y = rect.bottom - rect.bottom/9 - btnHeight/2;

    gGame.backButtonInfo.x = x;
    gGame.backButtonInfo.y = y;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SetBrushOrgEx(hdcBuffer, 0, 0, NULL);

    SelectObject(gRes.hdcMem, gRes.backButton);
    StretchBlt(hdcBuffer, x, y, btnWidth, btnHeight, gRes.hdcMem, 0, 0, btnWidth, btnHeight, SRCPAINT );

    SelectObject(gRes.hdcMem, gRes.backButtonMask);
    StretchBlt(hdcBuffer, x, y, btnWidth, btnHeight, gRes.hdcMem, 0, 0, btnWidth, btnHeight, SRCAND );

    HFONT oldFont = (HFONT) SelectObject(hdcBuffer, gRes.hFont);
    RECT textRect;
    textRect.left = x;
    textRect.top = y;
    textRect.right = x + btnWidth;
    textRect.bottom = y + btnHeight;

    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(50, 50, 50));
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdcBuffer, oldFont);

       // === RENDER BAKLJI ===
    float torchScale = 2.0f;

    int torchW = (int)(gGame.torchInfo.width  * torchScale);
    int torchH = (int)(gGame.torchInfo.height * torchScale);

    int torchMargin = 50;
    int torchX1 = torchMargin;
    int torchX2 = rect.right - torchW - torchMargin;
    int torchY = torchMargin + torchH/2;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    // Lijeva baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(
        hdcBuffer,
        torchX1, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCPAINT
    );
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(
        hdcBuffer,
        torchX1, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCAND
    );
    // Desna baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(
        hdcBuffer,
        torchX2, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCPAINT
    );
    SelectObject(gRes.hdcMem, gRes.torch);
   StretchBlt(
        hdcBuffer,
        torchX2, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCAND
    );

}

