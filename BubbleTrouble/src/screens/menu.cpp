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

    // Button dimensions - POVEĆANE DIMENZIJE
    int buttonWidth = screenWidth /8 ;      // Promenjeno sa /8 na /5 (veće)
    int buttonHeight = screenHeight / 8;   // Promenjeno sa /15 na /10 (veće)
    int buttonSpacing = 2;  // Promenjeno sa /25 na /20 (više razmaka)

    // Minimum sizes - POVEĆANI MINIMUMI
    if (buttonWidth < 150) buttonWidth = 150;   // Promenjeno sa 120 na 180
    if (buttonHeight < 50) buttonHeight = 50;   // Promenjeno sa 45 na 60
  //  if (buttonSpacing < 0) buttonSpacing = 0; // Promenjeno sa 20 na 25

    // Position menu on the LEFT side - responsive
    int leftSideCenter = clientRect.right / 4.5;  // Pomjeren kao prozor gore

    // Total height of all buttons
    int totalButtonsHeight = (3 * buttonHeight) ;

    // Center vertically with small offset down
    int startY = clientRect.bottom / 4.5;  // Gore kao prozor

    // 1 PLAYER button - centered on leftSideCenter
    gGame.menuButtons[0].rect.left = leftSideCenter - buttonWidth / 2;
    gGame.menuButtons[0].rect.top = startY;
    gGame.menuButtons[0].rect.right = leftSideCenter + buttonWidth / 2;
    gGame.menuButtons[0].rect.bottom = startY + buttonHeight;
    gGame.menuButtons[0].text = "1 PLAYER";
    gGame.menuButtons[0].isHovered = false;

    // 2 PLAYERS button - centered on leftSideCenter
    gGame.menuButtons[1].rect.left = leftSideCenter - buttonWidth / 2;
    gGame.menuButtons[1].rect.top = startY + (buttonHeight + buttonSpacing);
    gGame.menuButtons[1].rect.right = leftSideCenter + buttonWidth / 2;
    gGame.menuButtons[1].rect.bottom = startY + (buttonHeight + buttonSpacing) + buttonHeight;
    gGame.menuButtons[1].text = "2 PLAYERS";
    gGame.menuButtons[1].isHovered = false;

    // SETTINGS button - centered on leftSideCenter
    gGame.menuButtons[2].rect.left = leftSideCenter - buttonWidth / 2;
    gGame.menuButtons[2].rect.top = startY + 2 * (buttonHeight + buttonSpacing);
    gGame.menuButtons[2].rect.right = leftSideCenter + buttonWidth / 2;
    gGame.menuButtons[2].rect.bottom = startY + 2 * (buttonHeight + buttonSpacing) + buttonHeight;
    gGame.menuButtons[2].text = "SETTINGS";
    gGame.menuButtons[2].isHovered = false;
}

void RenderMenu(HDC hdc, RECT rect ) {

    // === KORISTIMO DIREKTNO TVOJ GLOBALNI BACKBUFFER (hdc)
    // === I GLOBALNI gRes.hdcMem

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

    if (gRes.torch && gRes.torchMask) {

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

            StretchBlt(hdc, torchX1, torchY, torchW, torchH,
                       gRes.hdcMem, torchSrcX, torchSrcY,
                       gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);

            SelectObject(gRes.hdcMem, gRes.torch);
            StretchBlt(hdc, torchX1, torchY, torchW, torchH,
                       gRes.hdcMem, torchSrcX, torchSrcY,
                       gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

            SelectObject(gRes.hdcMem, gRes.torchMask);
            StretchBlt(hdc, torchX2, torchY, torchW, torchH,
                       gRes.hdcMem, torchSrcX, torchSrcY,
                       gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);

            SelectObject(gRes.hdcMem, gRes.torch);
            StretchBlt(hdc, torchX2, torchY, torchW, torchH,
                       gRes.hdcMem, torchSrcX, torchSrcY,
                       gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

            SelectObject(gRes.hdcMem, oldMemBmp);
        }
    }

    // === LOGO ===
    if (gRes.logo && gRes.logoMask) {
        BITMAP bm;
        GetObject(gRes.logo, sizeof(BITMAP), &bm);

        int logoWidth = rect.right / 2;
        int logoHeight = (int)(logoWidth * ((float)bm.bmHeight / (float)bm.bmWidth));

        int maxLogoHeight = rect.bottom / 3;
        if (logoHeight > maxLogoHeight) {
            logoHeight = maxLogoHeight;
            logoWidth = (int)(logoHeight * ((float)bm.bmWidth / (float)bm.bmHeight));
        }

        int logoX = (rect.right - logoWidth) /1.7;
        int logoY = -20;

        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, gRes.logoMask);
        StretchBlt(hdc, logoX, logoY, logoWidth, logoHeight,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        SelectObject(gRes.hdcMem, gRes.logo);
        StretchBlt(hdc, logoX, logoY, logoWidth, logoHeight,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(gRes.hdcMem, oldMemBmp);
    }

    // === BUTTON HOLDER ===
    if (gRes.hButtonsHolder && gRes.hButtonsHolderMask) {
        BITMAP bm;
        GetObject(gRes.hButtonsHolder, sizeof(BITMAP), &bm);
        float aspectRatio = (float)bm.bmWidth / (float)bm.bmHeight;

        int buttonsTop = gGame.menuButtons[0].rect.top;
        int buttonsBottom = gGame.menuButtons[NUM_MENU_BUTTONS - 1].rect.bottom;
        int buttonsTotalHeight = buttonsBottom - buttonsTop;
        int oneButtonWidth = gGame.menuButtons[0].rect.right - gGame.menuButtons[0].rect.left;

        int holderWidth = (int)(oneButtonWidth * 2.8);
        int holderHeight = (int)(holderWidth / aspectRatio);

        if (holderHeight < buttonsTotalHeight * 1.7) {
            holderHeight = (int)(buttonsTotalHeight * 1.7);
            holderWidth = (int)(holderHeight * aspectRatio);
        }

        int buttonsCenterX = gGame.menuButtons[0].rect.left + (oneButtonWidth / 2);
        int holderX = buttonsCenterX - (holderWidth / 2);
        int buttonsCenterY = buttonsTop + buttonsTotalHeight / 2;
        int holderY = buttonsCenterY - (holderHeight / 2);

        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, gRes.hButtonsHolderMask);
        StretchBlt(hdc, holderX, holderY, holderWidth, holderHeight,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        SelectObject(gRes.hdcMem, gRes.hButtonsHolder);
        StretchBlt(hdc, holderX, holderY, holderWidth, holderHeight,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(gRes.hdcMem, oldMemBmp);
    }

    // === BUTTONS ===
    int buttonFontSize = max(14, rect.bottom / 35);
    HFONT hButtonFont = CreateFont(buttonFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                    VARIABLE_PITCH, TEXT("Arial"));
    HFONT hOldButtonFont = (HFONT)SelectObject(hdc, hButtonFont);

    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        MenuButton* btn = &gGame.menuButtons[i];
        HBITMAP currentButton = (i == 0) ? gRes.hYellowButton : (i == 1 ? gRes.hGreenButton : gRes.hGreyButton);
        HBITMAP currentButtonMask = (i == 0) ? gRes.hYellowButtonMask : (i == 1 ? gRes.hGreenButtonMask : gRes.hGreyButtonMask);

        BITMAP bm;
        GetObject(currentButton, sizeof(BITMAP), &bm);
        int btnWidth = btn->rect.right - btn->rect.left;
        int btnHeight = btn->rect.bottom - btn->rect.top;

        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, currentButtonMask);
        StretchBlt(hdc, btn->rect.left, btn->rect.top, btnWidth, btnHeight,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        SelectObject(gRes.hdcMem, currentButton);
        StretchBlt(hdc, btn->rect.left, btn->rect.top, btnWidth, btnHeight,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(gRes.hdcMem, oldMemBmp);

        SetBkMode(hdc, TRANSPARENT);
        SIZE textSize;
        GetTextExtentPoint32(hdc, btn->text, (int)strlen(btn->text), &textSize);
        int textX = btn->rect.left + ((btn->rect.right - btn->rect.left) - textSize.cx) / 2;
        int textY = btn->rect.top + ((btn->rect.bottom - btn->rect.top) - textSize.cy) / 2;

        SetTextColor(hdc, RGB(0, 0, 0));
        TextOut(hdc, textX + 2, textY + 2, btn->text, (int)strlen(btn->text));
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOut(hdc, textX, textY, btn->text, (int)strlen(btn->text));
    }

    SelectObject(hdc, hOldButtonFont);
    DeleteObject(hButtonFont);

    // === CHARACTER ===
    HBITMAP currentChar = gRes.menuCharacter;
    HBITMAP currentCharMask = gRes.menuCharacterMask;
    bool isPlayer2 = false;

    if (gGame.menuButtons[0].isHovered) {
        currentChar = gRes.player1MenuChar;
        currentCharMask = gRes.player1MenuCharMask;
    }
    else if (gGame.menuButtons[1].isHovered) {
        currentChar = gRes.player2MenuChar;
        currentCharMask = gRes.player2MenuCharMask;
        isPlayer2 = true;
    }
    else if (gGame.menuButtons[2].isHovered) {
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
            charX = gGame.menuButtons[0].rect.right + (-100);
            charY = rect.bottom - (int)(charH * 0.8);
        } else {
            charH = (int)(rect.bottom * 0.75);
            charW = (int)(charH * aspectRatio);
            charX = (int)(rect.right * 0.2);
            charY = rect.bottom - charH - max(20, rect.bottom / 30);
        }

        HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, currentCharMask);
        StretchBlt(hdc, charX, charY, charW, charH,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        SelectObject(gRes.hdcMem, currentChar);
        StretchBlt(hdc, charX, charY, charW, charH,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(gRes.hdcMem, oldMemBmp);
    }
}




void HandleMenuClick(HWND hwnd, int x, int y) {
    POINT pt = {x, y};

    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        if (PtInRect(&gGame.menuButtons[i].rect, pt)) {
            switch(i) {
                case 0: // 1 PLAYER
                    StartGame(hwnd);
                    break;
                case 1: // 2 PLAYERS
                    MessageBox(hwnd, "2 Player mode - Coming soon!", "Info", MB_OK | MB_ICONINFORMATION);
                    break;
                case 2: // SETTINGS
                    gGame.gameState.currentMode = GAME_MODE_SETTINGS;
                    //MessageBox(hwnd, "Settings - Coming soon!", "Info", MB_OK | MB_ICONINFORMATION);
                    break;
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

        if (wasHovered != gGame.menuButtons[i].isHovered) {
            needsRedraw = true;
        }
    }

    if (needsRedraw) {
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

