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

    SelectObject(gRes.hdcMem, gRes.hYellowButton);
    StretchBlt(hdcBuffer, x, y, btnWidth/2, btnHeight/2, gRes.hdcMem, 0, 0, btnWidth, btnHeight, SRCPAINT );

    SelectObject(gRes.hdcMem, gRes.hYellowButtonMask);
    StretchBlt(hdcBuffer, x, y, btnWidth/2, btnHeight/2, gRes.hdcMem, 0, 0, btnWidth, btnHeight, SRCAND );

    HFONT oldFont = (HFONT) SelectObject(hdcBuffer, gRes.hFont);
    RECT textRect;
    textRect.left = x;
    textRect.top = y;
    textRect.right = x + btnWidth/2;
    textRect.bottom = y + btnHeight/2;

    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(50, 50, 50));
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdcBuffer, oldFont);

}

