#include "resources.h"
#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include "globals.h"
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
    int buttonWidth = screenWidth / 5;      // Promenjeno sa /8 na /5 (veće)
    int buttonHeight = screenHeight / 6;   // Promenjeno sa /15 na /10 (veće)
    int buttonSpacing = -35;  // Promenjeno sa /25 na /20 (više razmaka)

    // Minimum sizes - POVEĆANI MINIMUMI
    if (buttonWidth < 200) buttonWidth = 200;   // Promenjeno sa 120 na 180
    if (buttonHeight < 70) buttonHeight = 70;   // Promenjeno sa 45 na 60
  //  if (buttonSpacing < 0) buttonSpacing = 0; // Promenjeno sa 20 na 25

    // Position menu on the LEFT side - responsive
    int leftSideCenter = clientRect.right / 3;  // 1/3 from left

    // Total height of all buttons
    int totalButtonsHeight = (3 * buttonHeight) ;

    // Center vertically with small offset down
    int startY = (clientRect.bottom - totalButtonsHeight) / 2 + clientRect.bottom / 10;

    // 1 PLAYER button - centered on leftSideCenter
    menuButtons[0].rect.left = leftSideCenter - buttonWidth / 2;
    menuButtons[0].rect.top = startY;
    menuButtons[0].rect.right = leftSideCenter + buttonWidth / 2;
    menuButtons[0].rect.bottom = startY + buttonHeight;
    menuButtons[0].text = "1 PLAYER";
    menuButtons[0].isHovered = false;

    // 2 PLAYERS button - centered on leftSideCenter
    menuButtons[1].rect.left = leftSideCenter - buttonWidth / 2;
    menuButtons[1].rect.top = startY + (buttonHeight + buttonSpacing);
    menuButtons[1].rect.right = leftSideCenter + buttonWidth / 2;
    menuButtons[1].rect.bottom = startY + (buttonHeight + buttonSpacing) + buttonHeight;
    menuButtons[1].text = "2 PLAYERS";
    menuButtons[1].isHovered = false;

    // SETTINGS button - centered on leftSideCenter
    menuButtons[2].rect.left = leftSideCenter - buttonWidth / 2;
    menuButtons[2].rect.top = startY + 2 * (buttonHeight + buttonSpacing);
    menuButtons[2].rect.right = leftSideCenter + buttonWidth / 2;
    menuButtons[2].rect.bottom = startY + 2 * (buttonHeight + buttonSpacing) + buttonHeight;
    menuButtons[2].text = "SETTINGS";
    menuButtons[2].isHovered = false;
}

void RenderMenu(HDC hdc, RECT rect ) {

    // Double buffering
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBufferBmp = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);
    SetStretchBltMode(hdcBuffer, HALFTONE);
    SetBrushOrgEx(hdcBuffer, 0, 0, NULL);

    // Render menu background
    if (gRes.menuScreen) {
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(hdcMem, gRes.menuScreen);
        BITMAP bm;
        GetObject(gRes.menuScreen, sizeof(BITMAP), &bm);
        StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(hdcMem, oldMemBmp);
    } else {
        // Fallback if no background - dark blue background
        HBRUSH hBrush = CreateSolidBrush(RGB(50, 50, 100));
        FillRect(hdcBuffer, &rect, hBrush);
        DeleteObject(hBrush);
    }


    // Render title "BUBBLE TROUBLE" - Responsive sizing
    SetBkMode(hdcBuffer, TRANSPARENT);
    int titleFontSize = max(40, rect.bottom / 10);
    HFONT hTitleFont = CreateFont(titleFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                   CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                   VARIABLE_PITCH, TEXT("Arial Black"));
    HFONT hOldTitleFont = (HFONT)SelectObject(hdcBuffer, hTitleFont);

    const char* title = "BUBBLE TROUBLE";
    SIZE titleSize;
    GetTextExtentPoint32(hdcBuffer, title, strlen(title), &titleSize);
    int titleX = (rect.right - titleSize.cx) / 2;
    int titleY = max(50, rect.bottom / 12);

    // Shadow for title
    SetTextColor(hdcBuffer, RGB(0, 0, 0));
    TextOut(hdcBuffer, titleX + 3, titleY + 3, title, strlen(title));

    // Main title
    SetTextColor(hdcBuffer, RGB(255, 255, 0));
    TextOut(hdcBuffer, titleX, titleY, title, strlen(title));

    SelectObject(hdcBuffer, hOldTitleFont);
    DeleteObject(hTitleFont);

    // === RENDER BUTTONS HOLDER FIRST (in background) ===
   // === RENDER BUTTONS HOLDER FIRST (in background) ===
    if (gRes.hButtonsHolder && gRes.hButtonsHolderMask) {
        BITMAP bm;
        GetObject(gRes.hButtonsHolder, sizeof(BITMAP), &bm);
        float aspectRatio = (float)bm.bmWidth / (float)bm.bmHeight;

        // 1. Izračunaj dimenzije dugmadi
        int buttonsTop = menuButtons[0].rect.top;
        int buttonsBottom = menuButtons[NUM_MENU_BUTTONS - 1].rect.bottom;
        int buttonsTotalHeight = buttonsBottom - buttonsTop;
        int oneButtonWidth = menuButtons[0].rect.right - menuButtons[0].rect.left;

        // 2. LOGIKA SKALIRANJA:
        // Prvo izračunaj veličinu holdera na osnovu ŠIRINE dugmeta
        // (neka holder bude 50% širi od dugmeta da ima lufta sa strane)
        int holderWidth = (int)(oneButtonWidth * 3);
        int holderHeight = (int)(holderWidth / aspectRatio);

        // 3. PROVJERA VISINE:
        // Ako je tako izračunat holder prenizak da stanu sva dugmad,
        // onda ga povećaj na osnovu visine dugmadi.
        if (holderHeight < buttonsTotalHeight * 1.2) { // 1.2 znači 20% lufta gore/dole
            holderHeight = (int)(buttonsTotalHeight * 1.2);
            holderWidth = (int)(holderHeight * aspectRatio);
        }

        // 4. Centriranje (ovo ostaje isto kao prije)
        int buttonsCenterX = menuButtons[0].rect.left + (oneButtonWidth / 2);
        int holderX = buttonsCenterX - (holderWidth / 2) + (rect.right / 75); // Ostavio sam tvoj mali offset

        int buttonsCenterY = buttonsTop + buttonsTotalHeight / 2;
        int holderY = buttonsCenterY - (holderHeight / 1.95);

        // Draw mask (SRCAND)
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(hdcMem, gRes.hButtonsHolderMask);
        StretchBlt(hdcBuffer, holderX, holderY, holderWidth, holderHeight,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        // Draw image (SRCPAINT)
        SelectObject(hdcMem, gRes.hButtonsHolder);
        StretchBlt(hdcBuffer, holderX, holderY, holderWidth, holderHeight,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(hdcMem, oldMemBmp);
    }

    // === RENDER BUTTONS WITH BITMAP BACKGROUND ===
    int buttonFontSize = max(16, rect.bottom / 30);  // Povećan font sa 18 na 20 i sa /25 na /20
    HFONT hButtonFont = CreateFont(buttonFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                    VARIABLE_PITCH, TEXT("Arial"));
    HFONT hOldButtonFont = (HFONT)SelectObject(hdcBuffer, hButtonFont);

    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        MenuButton* btn = &menuButtons[i];

        // === RENDER BUTTON BITMAP INSTEAD OF DRAWN RECTANGLE ===
        if (gRes.hMenuButton && gRes.hMenuButtonMask) {
            BITMAP bm;
            GetObject(gRes.hMenuButton, sizeof(BITMAP), &bm);

            // Dimenzije dugmeta iz rect
            int btnWidth = btn->rect.right - btn->rect.left;
            int btnHeight = btn->rect.bottom - btn->rect.top;

            // Render button background (mask first, then image)
            SetStretchBltMode(hdcBuffer, HALFTONE);
            SetBrushOrgEx(hdcBuffer, 0, 0, NULL);

            // Draw mask (SRCAND)
            HBITMAP oldMemBmp = (HBITMAP)SelectObject(hdcMem, gRes.hMenuButtonMask);
            StretchBlt(hdcBuffer, btn->rect.left, btn->rect.top, btnWidth, btnHeight,
                       hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

            // Draw button image (SRCPAINT)
            SelectObject(hdcMem, gRes.hMenuButton);
            StretchBlt(hdcBuffer, btn->rect.left, btn->rect.top, btnWidth, btnHeight,
                       hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

            SelectObject(hdcMem, oldMemBmp);

            /*// Optional: Add hover effect by drawing a semi-transparent overlay
            if (btn->isHovered) {
                // Lighter overlay for hover
                HBRUSH hHoverBrush = CreateSolidBrush(RGB(255, 255, 255));
                HPEN hHoverPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                HPEN hOldPen = (HPEN)SelectObject(hdcBuffer, hHoverPen);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcBuffer, hHoverBrush);

                // Set transparency (this won't work perfectly, but adds visual feedback)
                SetROP2(hdcBuffer, R2_MERGEPEN);
                Rectangle(hdcBuffer, btn->rect.left, btn->rect.top,
                         btn->rect.right, btn->rect.bottom);
                SetROP2(hdcBuffer, R2_COPYPEN);

                SelectObject(hdcBuffer, hOldPen);
                SelectObject(hdcBuffer, hOldBrush);
                DeleteObject(hHoverPen);
                DeleteObject(hHoverBrush);
            }*/
        } else {
            // Fallback: draw normal buttons if bitmap not loaded
            COLORREF btnColor;
            if (btn->isHovered) {
                btnColor = RGB(255, 100, 100);
            } else {
                btnColor = RGB(200, 50, 50);
            }

            HBRUSH hBtnBrush = CreateSolidBrush(btnColor);
            int borderWidth = max(2, rect.bottom / 350);
            HPEN hBtnPen = CreatePen(PS_SOLID, borderWidth, RGB(255, 255, 0));
            HPEN hOldPen = (HPEN)SelectObject(hdcBuffer, hBtnPen);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcBuffer, hBtnBrush);

            int cornerRadius = max(15, rect.bottom / 50);
            RoundRect(hdcBuffer, btn->rect.left, btn->rect.top,
                      btn->rect.right, btn->rect.bottom, cornerRadius, cornerRadius);

            SelectObject(hdcBuffer, hOldPen);
            SelectObject(hdcBuffer, hOldBrush);
            DeleteObject(hBtnPen);
            DeleteObject(hBtnBrush);
        }

        // === RENDER TEXT ON TOP OF BUTTON ===
        // Button text shadow
        SetTextColor(hdcBuffer, RGB(0, 0, 0));
        SIZE textSize;
        GetTextExtentPoint32(hdcBuffer, btn->text, strlen(btn->text), &textSize);
        int textX = btn->rect.left + ((btn->rect.right - btn->rect.left) - textSize.cx) / 2;
        int textY = btn->rect.top + ((btn->rect.bottom - btn->rect.top) - textSize.cy) / 2;
        TextOut(hdcBuffer, textX + 2, textY + 2, btn->text, strlen(btn->text));

        // Button text (main)
        SetTextColor(hdcBuffer, RGB(255, 255, 0));
        TextOut(hdcBuffer, textX, textY, btn->text, strlen(btn->text));
    }

    SelectObject(hdcBuffer, hOldButtonFont);
    DeleteObject(hButtonFont);

    // === RENDER MENU CHARACTER ON RIGHT SIDE - RESPONSIVE ===
    if (gRes.menuCharacter && gRes.menuCharacterMask) {
        BITMAP bm;
        GetObject(gRes.menuCharacter, sizeof(BITMAP), &bm);
        int charHeight = (int)(rect.bottom * 0.75);
        float aspectRatio = (float)bm.bmWidth / (float)bm.bmHeight;
        int charWidth = (int)(charHeight * aspectRatio);

        // Position on right side
        int charX = rect.right - charWidth - max(30, rect.right / 20);
        int charY = rect.bottom - charHeight - max(20, rect.bottom / 30);

        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);

        // Draw mask first (for transparency)
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(hdcMem, gRes.menuCharacterMask);
        StretchBlt(hdcBuffer, charX, charY, charWidth, charHeight,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        // Draw character
        SelectObject(hdcMem, gRes.menuCharacter);
        StretchBlt(hdcBuffer, charX, charY, charWidth, charHeight,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(hdcMem, oldMemBmp);
    }

    // Copy to screen
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuffer, 0, 0, SRCCOPY);

    // Cleanup
    SelectObject(hdcBuffer, oldBufferBmp);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcMem);
    DeleteDC(hdcBuffer);
}

void HandleMenuClick(HWND hwnd, int x, int y) {
    POINT pt = {x, y};

    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        if (PtInRect(&menuButtons[i].rect, pt)) {
            switch(i) {
                case 0: // 1 PLAYER
                    StartGame(hwnd);
                    break;
                case 1: // 2 PLAYERS
                    MessageBox(hwnd, "2 Player mode - Coming soon!", "Info", MB_OK | MB_ICONINFORMATION);
                    break;
                case 2: // SETTINGS
                    MessageBox(hwnd, "Settings - Coming soon!", "Info", MB_OK | MB_ICONINFORMATION);
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
        bool wasHovered = menuButtons[i].isHovered;
        menuButtons[i].isHovered = PtInRect(&menuButtons[i].rect, pt);

        if (wasHovered != menuButtons[i].isHovered) {
            needsRedraw = true;
        }
    }

    if (needsRedraw) {
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void StartGame(HWND hwnd) {
    gameState.currentMode = GAME_MODE_PLAYING;
    ResetGame(hwnd);
    InvalidateRect(hwnd, NULL, FALSE);
}

void ResetGame(HWND hwnd) {
    // Reset game state
    gameState.timeLeft = maxTime;
    gameState.isGameOver = false;
    gameState.isLevelCleared = false;
    gameState.activeBalloonCount = 0;
    gameState.lives = MAX_LIVES;

    // Reset hero position
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    hero.x = leftWall.width + hero.width;
    hero.y = 100;
    hero.currentRow = 2;
    hero.currentFrame = 0;

    // Reset harpoon
    harpoon.isActive = false;
    harpoon.length = 0;

    // Clear all balloons
    for (int i = 0; i < MAX_BALLOONS; i++) {
        balloons[i].active = false;
    }

    // Initialize starting balloons
    InitBalloon(1, 400, 150, 40, -2.0f, RGB(0, 255, 0));
}
