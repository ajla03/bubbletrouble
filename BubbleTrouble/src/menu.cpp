#include "resources.h"
#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include "globals.h"
#include "game.h"

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

    // Button dimensions - responsive
    int buttonWidth = screenWidth / 8;
    int buttonHeight = screenHeight / 15;
    int buttonSpacing = screenHeight / 25;

    // Minimum sizes
    if (buttonWidth < 120) buttonWidth = 120;
    if (buttonHeight < 45) buttonHeight = 45;
    if (buttonSpacing < 20) buttonSpacing = 20;

    // Position menu on the LEFT side - responsive
    int leftSideCenter = clientRect.right / 3;  // 1/3 from left

    // Total height of all buttons
    int totalButtonsHeight = (3 * buttonHeight) + (2 * buttonSpacing);

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

void RenderMenu(HDC hdcBuffer, RECT rect) {
    HDC hdcMem = CreateCompatibleDC(hdcBuffer);

    // Render menu background
    if (menuScreen) {
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(hdcMem, menuScreen);
        BITMAP bm;
        GetObject(menuScreen, sizeof(BITMAP), &bm);
        StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(hdcMem, oldMemBmp);
    } else {
        // Fallback if no background - dark blue background
        HBRUSH hBrush = CreateSolidBrush(RGB(50, 50, 100));
        FillRect(hdcBuffer, &rect, hBrush);
        DeleteObject(hBrush);
    }

    // === RENDER BUTTONS HOLDER FIRST (in background) ===
    if (hButtonsHolder && hButtonsHolderMask) {
        BITMAP bm;
        GetObject(hButtonsHolder, sizeof(BITMAP), &bm);

        int btnWidth = menuButtons[0].rect.right - menuButtons[0].rect.left;
        int buttonsTop = menuButtons[0].rect.top;
        int buttonsBottom = menuButtons[NUM_MENU_BUTTONS - 1].rect.bottom;
        int buttonsHeight = buttonsBottom - buttonsTop;

        int holderWidth = (int)(btnWidth * 1.6);
        float aspectRatio = (float)bm.bmHeight * 1.1 / (float)bm.bmWidth;
        int holderHeight = (int)(holderWidth * aspectRatio);

        int holderX = (rect.right / 3) - (holderWidth / 2.1);
        int buttonsCenterY = buttonsTop + buttonsHeight / 2;
        int holderY = buttonsCenterY - (holderHeight / 1.7);

        // Draw mask (SRCAND)
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(hdcMem, hButtonsHolderMask);
        StretchBlt(hdcBuffer, holderX, holderY, holderWidth, holderHeight,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        // Draw image (SRCPAINT)
        SelectObject(hdcMem, hButtonsHolder);
        StretchBlt(hdcBuffer, holderX, holderY, holderWidth, holderHeight,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(hdcMem, oldMemBmp);
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

    // Render buttons - Responsive font size
    int buttonFontSize = max(18, rect.bottom / 25);
    HFONT hButtonFont = CreateFont(buttonFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                    VARIABLE_PITCH, TEXT("Arial"));
    HFONT hOldButtonFont = (HFONT)SelectObject(hdcBuffer, hButtonFont);

    for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
        MenuButton* btn = &menuButtons[i];

        // Button background with gradient effect
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

        // Button text shadow
        SetTextColor(hdcBuffer, RGB(0, 0, 0));
        SIZE textSize;
        GetTextExtentPoint32(hdcBuffer, btn->text, strlen(btn->text), &textSize);
        int textX = btn->rect.left + ((btn->rect.right - btn->rect.left) - textSize.cx) / 2;
        int textY = btn->rect.top + ((btn->rect.bottom - btn->rect.top) - textSize.cy) / 2;
        TextOut(hdcBuffer, textX + 2, textY + 2, btn->text, strlen(btn->text));

        // Button text
        SetTextColor(hdcBuffer, RGB(255, 255, 0));
        TextOut(hdcBuffer, textX, textY, btn->text, strlen(btn->text));
    }

    SelectObject(hdcBuffer, hOldButtonFont);
    DeleteObject(hButtonFont);

    // === RENDER MENU CHARACTER ON RIGHT SIDE - RESPONSIVE ===
    if (menuCharacter && menuCharacterMask) {
        BITMAP bm;
        GetObject(menuCharacter, sizeof(BITMAP), &bm);

        // Responsive character size
        int charSize = max(150, min(rect.bottom / 2, rect.right / 4));
        int charX = rect.right - charSize - max(50, rect.right / 10);
        int charY = rect.bottom - charSize - max(30, rect.bottom / 25);

        // Draw mask first (for transparency)
        HBITMAP oldMemBmp = (HBITMAP)SelectObject(hdcMem, menuCharacterMask);
        StretchBlt(hdcBuffer, charX, charY, charSize, charSize,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

        // Draw character
        SelectObject(hdcMem, menuCharacter);
        StretchBlt(hdcBuffer, charX, charY, charSize, charSize,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

        SelectObject(hdcMem, oldMemBmp);
    }

    // Cleanup
    DeleteDC(hdcMem);
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
    gameState.currentMode = GAME_MODE_PLAYING;
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
