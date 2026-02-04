#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include <wingdi.h>

void RenderSettings(HDC hdcBuffer, RECT rect)
{
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom,
               gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);

    // --- PROZIRNI SHEET ---
    RECT sheet;
    sheet.left = rect.right / 8;
    sheet.right = rect.right - rect.right / 8;
    sheet.top = rect.bottom / 8;
    sheet.bottom = rect.bottom - rect.bottom / 8;


    HBITMAP bmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
    HBITMAP hBmpOld = (HBITMAP) SelectObject(gRes.hdcMem, bmp);

    SetPixel(gRes.hdcMem, 0, 0, RGB(255, 255, 255));

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 160;
    bf.AlphaFormat = 0;

    AlphaBlend(hdcBuffer, sheet.left, sheet.top,
               sheet.right - sheet.left, sheet.bottom - sheet.top,
               gRes.hdcMem, 0, 0, 1, 1, bf);

    SelectObject(gRes.hdcMem, hBmpOld);
    DeleteObject(bmp);

    // SETTINGS TEKST //
    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFont);
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(60, 60, 60));
    RECT textRect;
    textRect.left   = sheet.left;
    textRect.right  = sheet.right;
    textRect.top    = sheet.top + 20;
    textRect.bottom = sheet.top + 60;

    DrawText(
        hdcBuffer,
        "SETTINGS",
        -1,
        &textRect,
        DT_CENTER | DT_TOP | DT_SINGLELINE
    );


    // --- BACK BUTTON ---
    int padding = 15;
    int btnWidth  = gGame.backButtonInfo.width;
    int btnHeight = gGame.backButtonInfo.height;
    int x = sheet.left + padding;
    int y = sheet.bottom - btnHeight/2 - padding;

    gGame.backButtonInfo.x = x;
    gGame.backButtonInfo.y = y;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SelectObject(gRes.hdcMem, gRes.backButton);
    StretchBlt(hdcBuffer, x, y, btnWidth, btnHeight/2,
               gRes.hdcMem, 0, 0, btnWidth, btnHeight, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.backButtonMask);
    StretchBlt(hdcBuffer, x, y, btnWidth, btnHeight/2,
               gRes.hdcMem, 0, 0, btnWidth, btnHeight, SRCAND);

    textRect = { x, y, x + btnWidth, y + btnHeight/2 };
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdcBuffer, oldFont);

    // --- TORCHES ---
    float torchScale = 2.0f;
    int torchW = (int)(gGame.torchInfo.width * torchScale);
    int torchH = (int)(gGame.torchInfo.height * torchScale);

    int torchX1 = sheet.left/2 -torchW/2;
    int torchX2 = sheet.right + sheet.left/2 - torchW/2;
    int torchY = sheet.bottom/2;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY,
               gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY,
               gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY,
               gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY,
               gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

    // --- SOUND HOLDER (center) ---
    BITMAP bm;
    GetObject(gRes.hSoundHolder, sizeof(BITMAP), &bm);

    float soundScale = 0.5;
    int soundW = bm.bmWidth*soundScale;
    int soundH = bm.bmHeight*soundScale;

    x = sheet.right - soundW - padding;
    y = sheet.bottom - soundH - padding;

    SelectObject(gRes.hdcMem, gRes.hSoundHolder);
    StretchBlt(hdcBuffer, x, y, soundW, soundH,
               gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);
    SelectObject(gRes.hdcMem, gRes.hSoundHolderMask);
    StretchBlt(hdcBuffer, x, y, soundW, soundH,
               gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
}


