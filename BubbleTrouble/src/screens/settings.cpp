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

    /* CONTROLS */
    BITMAP bm;
    int padding = 15;
    GetObject(gRes.controlsHolder, sizeof(BITMAP), &bm);
    int controlW = bm.bmWidth/1.5;
    int controlH = bm.bmHeight/1.5;
    int x = sheet.right - controlW - padding;
    int y =  padding + textRect.bottom;

    SelectObject(gRes.hdcMem, gRes.controlsHolder);
    TransparentBlt(hdcBuffer, x, y, controlW, controlH,
               gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
               RGB(255, 255, 255));

    /* HEROJI UNUTAR HOLDERA */
    int heroW = gGame.hero.width * 1.5;
    int heroH = gGame.hero.height * 1.5;
    int spacing = 30;


    int heroY = y + (controlH / 2) - (heroH / 2) - 10;
    int midHeroX = x + (controlW / 2) - (heroW / 2);
    int leftHeroX = midHeroX - heroW - spacing;
    int rightHeroX = midHeroX + heroW + spacing;

    // --- LIJEVO---
    int srcX_left = 0 * gGame.hero.width;
    int srcY_left = 1 * gGame.hero.height;

    SetStretchBltMode(hdcBuffer, COLORONCOLOR);
    SelectObject(gRes.hdcMem, gRes.characterMask);
    StretchBlt(hdcBuffer, leftHeroX, heroY, heroW, heroH, gRes.hdcMem, srcX_left, srcY_left, gGame.hero.width, gGame.hero.height, SRCAND);
    SelectObject(gRes.hdcMem, gRes.character);
    StretchBlt(hdcBuffer, leftHeroX, heroY, heroW, heroH, gRes.hdcMem, srcX_left, srcY_left, gGame.hero.width, gGame.hero.height, SRCPAINT);

    // --- SREDINA---
    int srcX_back = 0 * gGame.hero.width;
    int srcY_back = 2 * gGame.hero.height;

    SelectObject(gRes.hdcMem, gRes.characterMask);
    StretchBlt(hdcBuffer, midHeroX, heroY, heroW, heroH, gRes.hdcMem, srcX_back, srcY_back, gGame.hero.width, gGame.hero.height, SRCAND);
    SelectObject(gRes.hdcMem, gRes.character);
    StretchBlt(hdcBuffer, midHeroX, heroY, heroW, heroH, gRes.hdcMem, srcX_back, srcY_back, gGame.hero.width, gGame.hero.height, SRCPAINT);

    // --- DESNO---
    int srcX_right = 3* gGame.hero.width;
    int srcY_right = 0 * gGame.hero.height;

    SelectObject(gRes.hdcMem, gRes.characterMask);
    StretchBlt(hdcBuffer, rightHeroX, heroY, heroW, heroH, gRes.hdcMem, srcX_right, srcY_right, gGame.hero.width, gGame.hero.height, SRCAND);
    SelectObject(gRes.hdcMem, gRes.character);
    StretchBlt(hdcBuffer, rightHeroX, heroY, heroW, heroH, gRes.hdcMem, srcX_right, srcY_right, gGame.hero.width, gGame.hero.height, SRCPAINT);

    /* DUGMAD (BUTTONS) ISPOD HEROJA */

    int btnW = 60;
    int btnH = 30;
    int btnOffsetY = heroH + padding;

    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    SetBkMode(hdcBuffer, TRANSPARENT);

    const char* btnTexts[] = { "<-", "SPC", "->" };
    int btnXPositions[] = { leftHeroX + (heroW/2) - (btnW/2),
                            midHeroX + (heroW/2) - (btnW/2),
                            rightHeroX + (heroW/2) - (btnW/2) };

    for (int i = 0; i < 3; i++) {
        int currentBtnX = btnXPositions[i];
        int currentBtnY = heroY + btnOffsetY;

        BITMAP bmBtn;
        GetObject(gRes.settingsPlayer, sizeof(BITMAP), &bmBtn);

        SelectObject(gRes.hdcMem, gRes.settingsPlayer);
        TransparentBlt(hdcBuffer, currentBtnX, currentBtnY, btnW, btnH,
                       gRes.hdcMem, 0, 0, bmBtn.bmWidth, bmBtn.bmHeight,
                       RGB(255, 255, 255));

        RECT btnRect = { currentBtnX, currentBtnY, currentBtnX + btnW, currentBtnY + btnH };
        DrawText(hdcBuffer, btnTexts[i], -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    // --- BACK BUTTON ---
    int btnWidth  = gGame.backButtonInfo.width;
    int btnHeight = gGame.backButtonInfo.height;
    x = sheet.left + padding;
    y = sheet.bottom - btnHeight/2 - padding;

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


