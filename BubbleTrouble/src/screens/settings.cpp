#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "constants.h"
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
    sheet.left   = rect.right / 2 - SHEET_W / 2;
    sheet.right  = sheet.left + SHEET_W;
    sheet.top    = rect.bottom / 2 - SHEET_H / 2;
    sheet.bottom = sheet.top + SHEET_H;

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
    int padding = 40;
    GetObject(gRes.controlsHolder, sizeof(BITMAP), &bm);
    int controlW = bm.bmWidth/1.5;
    int controlH = bm.bmHeight/1.5;
    int x = sheet.right - controlW - padding;
    int y =  15 + textRect.bottom;

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
    int btnOffsetY = heroH + 15;

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

    // PLAYER BUTTONS //
    int btnWidth = gGame.backButtonInfo.width;
    int btnHeight = gGame.backButtonInfo.height;
    btnW = gGame.backButtonInfo.width * 1.5;
    btnH = gGame.backButtonInfo.height / 2;
    int btnX = x/2 - btnW/2 + sheet.left/2;
    int groupHeight = btnH * 2 + padding;
    int groupTop = y + (controlH - groupHeight) / 2;

    int firstBtnY  = groupTop;
    int secondBtnY = groupTop + btnH + padding;

    // ===== PLAYER 1 =====
    SelectObject(gRes.hdcMem, gRes.backButtonMask);
    TransparentBlt(
        hdcBuffer, btnX, firstBtnY, btnW, btnH,
        gRes.hdcMem, 0, 0,
        btnWidth,
        btnHeight,
        RGB(255,255,255)
    );

    RECT r1 = { btnX, firstBtnY, btnX + btnW, firstBtnY + btnH };
    DrawText(hdcBuffer, "PLAYER 1", -1, &r1,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);


    // ===== PLAYER 2 =====
    TransparentBlt(
        hdcBuffer, btnX, secondBtnY, btnW, btnH,
        gRes.hdcMem, 0, 0,
        btnWidth,
        btnHeight,
        RGB(255,255,255)
    );

    RECT r2 = { btnX, secondBtnY, btnX + btnW, secondBtnY + btnH };
    DrawText(hdcBuffer, "PLAYER 2", -1, &r2,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // --- BACK BUTTON ---

    btnWidth = gGame.backButtonInfo.width;
    btnHeight = gGame.backButtonInfo.height;
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

    // --- SOUND HOLDER ---
    GetObject(gRes.hMusicHolder, sizeof(BITMAP), &bm);

    float soundScale = 0.5;
    int soundW = bm.bmWidth*soundScale;
    int soundH = bm.bmHeight*soundScale;

    x = sheet.right - soundW - padding;
    y = sheet.bottom - soundH - padding;

    SelectObject(gRes.hdcMem, gRes.hMusicHolder);
    StretchBlt(hdcBuffer, x, y, soundW, soundH,
               gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);
    SelectObject(gRes.hdcMem, gRes.hMusicHolderMask);
    StretchBlt(hdcBuffer, x, y, soundW, soundH,
               gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

    SelectObject(gRes.hdcMem, gRes.hSoundOn);
    TransparentBlt(hdcBuffer, x, y - padding - soundH, soundW, soundH,
                       gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
                       RGB(255, 255, 255));


    // --- SOUND BUTTON UNUTAR SOUND HOLDERA ---
    GetObject(gRes.soundButton, sizeof(BITMAP), &bm);

    int soundBtnW = bm.bmWidth * 0.4;
    int soundBtnH = bm.bmHeight * 0.4;
    int soundBtnX = x + (soundW - soundBtnW) / 2;
    int soundBtnY = y + (soundH - soundBtnH) / 2;

    gGame.settingsSoundButtonInfo.x = soundBtnX;
    gGame.settingsSoundButtonInfo.y = soundBtnY;
    gGame.settingsSoundButtonInfo.width = soundBtnW;
    gGame.settingsSoundButtonInfo.height = soundBtnH;

    if (!gGame.soundState.bgMusicOn) {
        HBITMAP overlayBmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
        HBITMAP hOldOverlay = (HBITMAP) SelectObject(gRes.hdcMem, overlayBmp);
        SetPixel(gRes.hdcMem, 0, 0, RGB(100, 100, 100));

        BLENDFUNCTION bfOverlay = {};
        bfOverlay.BlendOp = AC_SRC_OVER;
        bfOverlay.SourceConstantAlpha = 120;
        bfOverlay.AlphaFormat = 0;

        AlphaBlend(hdcBuffer, soundBtnX, soundBtnY, soundBtnW, soundBtnH,
                   gRes.hdcMem, 0, 0, 1, 1, bfOverlay);

        SelectObject(gRes.hdcMem, hOldOverlay);
        DeleteObject(overlayBmp);

        HPEN hRedPen = CreatePen(PS_SOLID, 3, RGB(255, 50, 50));
        HPEN hOldPen = (HPEN)SelectObject(hdcBuffer, hRedPen);

        MoveToEx(hdcBuffer, soundBtnX, soundBtnY, NULL);
        LineTo(hdcBuffer, soundBtnX + soundBtnW, soundBtnY + soundBtnH);
        MoveToEx(hdcBuffer, soundBtnX + soundBtnW, soundBtnY, NULL);
        LineTo(hdcBuffer, soundBtnX, soundBtnY + soundBtnH);

        SelectObject(hdcBuffer, hOldPen);
        DeleteObject(hRedPen);
    }

    if (gGame.settingsSoundButtonInfo.isHover) {
        HBITMAP hoverBmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
        HBITMAP hOldHover = (HBITMAP) SelectObject(gRes.hdcMem, hoverBmp);
        SetPixel(gRes.hdcMem, 0, 0, RGB(255, 255, 255));

        BLENDFUNCTION bfHover = {};
        bfHover.BlendOp = AC_SRC_OVER;
        bfHover.SourceConstantAlpha = 50;
        bfHover.AlphaFormat = 0;

        AlphaBlend(hdcBuffer, soundBtnX, soundBtnY, soundBtnW, soundBtnH,
                   gRes.hdcMem, 0, 0, 1, 1, bfHover);

        SelectObject(gRes.hdcMem, hOldHover);
        DeleteObject(hoverBmp);
    }
}
