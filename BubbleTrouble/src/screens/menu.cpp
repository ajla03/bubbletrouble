#include "resources.h"
#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include "gameContext.h"
#include "game.h"
#include "resourceManager.h"

// Helper macros if std::max/min don't work
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

void InitializeMenu(HWND hwnd) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    int screenHeight = clientRect.bottom;
    int screenWidth = clientRect.right;

    float holderCenterX = screenWidth * 0.225f;
    float holderCenterY = screenHeight * 0.416f;
    int holderWidth = (int)(screenWidth * 0.233f);
    int holderHeight = (int)(screenHeight * 0.460f);

    int buttonWidth = (int)(holderWidth * 0.65f);
    int buttonHeight = screenHeight / 14;

    if (buttonWidth < 150) buttonWidth = 150;
    if (buttonHeight < 60) buttonHeight = 60;

    int buttonSpacing = (holderHeight - (3 * buttonHeight)) / 4;
    if (buttonSpacing < 10) buttonSpacing = 10;

    int totalGroupHeight = (3 * buttonHeight) + (2 * buttonSpacing);
    int startY = (int)holderCenterY - (totalGroupHeight / 2);

    for (int i = 0; i < 3; i++) {
        gGame.menuButtons[i].rect.left = holderCenterX - (buttonWidth / 2);
        gGame.menuButtons[i].rect.right = holderCenterX + (buttonWidth / 2);
        gGame.menuButtons[i].rect.top = startY + i * (buttonHeight + buttonSpacing);
        gGame.menuButtons[i].rect.bottom = gGame.menuButtons[i].rect.top + buttonHeight;
    }

    gGame.menuButtons[0].text = "1 PLAYER";
    gGame.menuButtons[1].text = "2 PLAYERS";
    gGame.menuButtons[2].text = "SETTINGS";

    gGame.helpIconHovered = false;
}

void RenderMenu(HDC hdc, RECT rect) {
    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, NULL);

    // Render menu background
    if (gRes.menuScreen) {
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, gRes.menuScreen);
        BITMAP bm;
        GetObject(gRes.menuScreen, sizeof(BITMAP), &bm);
        StretchBlt(hdc, 0, 0, rect.right, rect.bottom,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(gRes.hdcMem, oldMemBmp);
    } else {
        HBRUSH hBrush = CreateSolidBrush(RGB(50, 50, 100));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
    }

    // === TORCHES ===
    /*if (gRes.torch && gRes.torchMask) {
        float torchScale = 3.5f;
        if (rect.right < 1400) torchScale = 2.5f;
        if (rect.right < 900)  torchScale = 1.8f;

        if (rect.right > 600) {
            int torchW = (int)(gGame.torchInfo.width * torchScale);
            int torchH = (int)(gGame.torchInfo.height * torchScale);
            int padding = (rect.right < 1000) ? 5 : 20;
            int liftUpAmount = rect.bottom / 6;
            int torchY = (rect.bottom / 2) - (torchH / 2) - liftUpAmount;
            int torchX1 = padding;
            int torchX2 = rect.right - torchW - padding;

            int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
            int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;


            HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, gRes.torchMask);
            StretchBlt(hdc, torchX1, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
            SelectObject(gRes.hdcMem, gRes.torch);
            StretchBlt(hdc, torchX1, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

            SelectObject(gRes.hdcMem, gRes.torchMask);
            StretchBlt(hdc, torchX2, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
            SelectObject(gRes.hdcMem, gRes.torch);
            StretchBlt(hdc, torchX2, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

            SelectObject(gRes.hdcMem, oldMemBmp);

        }
    }*/
/*
  // === LOGO (ICON) ===
if (gRes.hIcon) {  // pretpostavljam da imate HICON u gRes strukturi
    int logoWidth = rect.right / 2;
    int logoHeight = logoWidth;  // ikone su obično kvadratne
    int maxLogoHeight = rect.bottom / 3;

    if (logoHeight > maxLogoHeight) {
        logoHeight = maxLogoHeight;
        logoWidth = maxLogoHeight;  // održava kvadratni oblik
    }

    int logoX = (rect.right - logoWidth) / 5.8;
    int logoY = -20;

    // Crta ikonu sa automatskom transparencijom
    DrawIconEx(hdc, logoX, logoY, gRes.hIcon,
               logoWidth, logoHeight,
               0,           // frame index (za animirane ikone)
               NULL,        // bez background brush-a
               DI_NORMAL);  // normalan crtež sa transparencijom
}*/

    // === BUTTONS ===
    // Use the same font as settings screen
    HFONT hOldButtonFont = (HFONT)SelectObject(hdc, gRes.hFont);

    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        MenuButton* btn = &gGame.menuButtons[i];

        // Koristi iste buttone kao u settings-u
        HBITMAP currentButton = btn->isHovered ? gRes.settingsPlayer : gRes.playerHover;

        BITMAP bm;
        GetObject(currentButton, sizeof(BITMAP), &bm);
        int btnWidth = btn->rect.right - btn->rect.left;
        int btnHeight = btn->rect.bottom - btn->rect.top;

        // Koristi TransparentBlt umjesto SRCAND/SRCPAINT kao u settings-u
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, currentButton);
        TransparentBlt(hdc, btn->rect.left, btn->rect.top, btnWidth, btnHeight,
                       gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));
        SelectObject(gRes.hdcMem, oldMemBmp);

        // Tekst na buttonu
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));

        RECT textRect = { btn->rect.left, btn->rect.top, btn->rect.right, btn->rect.bottom };
        DrawText(hdc, btn->text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc, hOldButtonFont);
    // === HELP ICON ===
    if (gRes.help && gRes.helpHover) {
        HBITMAP currentHelp = gGame.helpIconHovered ? gRes.helpHover : gRes.help;

        BITMAP bm;
        GetObject(currentHelp, sizeof(BITMAP), &bm);

        int helpSize = rect.right / 20;
        if (helpSize < 40) helpSize = 40;
        if (helpSize > 80) helpSize = 80;

        int padding = 20;
        int helpX = rect.right - helpSize - padding;
        int helpY = padding;

        gGame.helpIconRect.left = helpX;
        gGame.helpIconRect.top = helpY;
        gGame.helpIconRect.right = helpX + helpSize;
        gGame.helpIconRect.bottom = helpY + helpSize;

        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, currentHelp);
        TransparentBlt(hdc, helpX, helpY, helpSize, helpSize,
                       gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
                       RGB(255, 255, 255));
        SelectObject(gRes.hdcMem, oldMemBmp);
    }

     // === PODIUM ICON ===
    if (gRes.podium && gRes.podiumHover) {
        BITMAP bm;
        HBITMAP current = gGame.dashboardButtonInfo.isHover ? gRes.podiumHover : gRes.podium;
        GetObject(current, sizeof(BITMAP), &bm);

        int podiumSize = rect.right / 20;
        if (podiumSize < 40) podiumSize = 40;
        if (podiumSize > 80) podiumSize = 80;

        int padding = 20;
        int helpX = rect.right - podiumSize - padding;
        int helpY = 2*padding + podiumSize;

        gGame.dashboardButtonInfo.x = helpX;
        gGame.dashboardButtonInfo.y = helpY;
        gGame.dashboardButtonInfo.width = podiumSize;
        gGame.dashboardButtonInfo.height = podiumSize;

        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem,current);

        TransparentBlt(hdc, helpX, helpY, podiumSize, podiumSize,
                       gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
                       RGB(255, 255, 255));
        SelectObject(gRes.hdcMem, oldMemBmp);
    }


    // === CHARACTER ===
    HBITMAP currentChar = gRes.menuCharacter;
    HBITMAP currentCharMask = gRes.menuCharacterMask;
    bool isPlayer2 = false;

    if (gGame.menuButtons[0].isHovered) {
        currentChar = gRes.player1MenuChar;
        currentCharMask = gRes.player1MenuCharMask;
    } else if (gGame.menuButtons[1].isHovered) {
        currentChar = gRes.player2MenuChar;
        currentCharMask = gRes.player2MenuCharMask;
        isPlayer2 = true;
    } else if (gGame.menuButtons[2].isHovered) {
        currentChar = gRes.settingsMenuChar;
        currentCharMask = gRes.settingsMenuCharMask;
    }

    if (currentChar && currentCharMask) {
        BITMAP bm;
        GetObject(currentChar, sizeof(BITMAP), &bm);
        float aspectRatio = (float)bm.bmWidth / (float)bm.bmHeight;
        int charW, charH, charX, charY;

        if (isPlayer2) {
            charH = (int)(rect.bottom * 1.3);
            charW = (int)(charH * aspectRatio);
            charX = gGame.menuButtons[0].rect.right + 100;
            charY = rect.bottom - (int)(charH * 0.78);
        } else {
            charH = (int)(rect.bottom * 0.75);
            charW = (int)(charH * aspectRatio);
            charX = gGame.menuButtons[0].rect.right + 100;
            charY = rect.bottom - charH - max(20, rect.bottom / 30);
        }

        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, currentCharMask);
        StretchBlt(hdc, charX, charY, charW, charH, gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);
        SelectObject(gRes.hdcMem, currentChar);
        StretchBlt(hdc, charX, charY, charW, charH, gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
        SelectObject(gRes.hdcMem, oldMemBmp);
    }
}

void HandleMenuClick(HWND hwnd, int x, int y) {
    POINT pt = {x, y};
     if (PtInRect(&gGame.helpIconRect, pt)) {
        MessageBox(hwnd, "Help clicked!", "Info", MB_OK); // Privremeno
        return;
    }
    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        if (PtInRect(&gGame.menuButtons[i].rect, pt)) {
            switch(i) {
                case 0: gGame.gameState.isMultiplayer = false; StartGame(hwnd); gGame.menuButtons[0].isHovered = false; break;
                case 1: gGame.gameState.isMultiplayer = true; StartGame(hwnd); InitMultiplayer(hwnd); gGame.menuButtons[1].isHovered = false;  break;
                case 2: gGame.gameState.currentMode = GAME_MODE_SETTINGS; gGame.menuButtons[2].isHovered = false; break;
            }
            break;
        }
    }
}

void HandleMenuMouseMove(HWND hwnd, int x, int y) {
    POINT pt = {x, y};
    bool needsRedraw = false;
    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        bool wasHovered = gGame.menuButtons[i].isHovered;
        gGame.menuButtons[i].isHovered = PtInRect(&gGame.menuButtons[i].rect, pt);
        if (wasHovered != gGame.menuButtons[i].isHovered) needsRedraw = true;
    }
      bool wasHelpHovered = gGame.helpIconHovered;
    gGame.helpIconHovered = PtInRect(&gGame.helpIconRect, pt);
    if (wasHelpHovered != gGame.helpIconHovered) needsRedraw = true;

    if (needsRedraw) InvalidateRect(hwnd, NULL, FALSE);

}
