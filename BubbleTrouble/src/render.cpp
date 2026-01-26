#include "resources.h"
#include <windows.h>
#include <algorithm>

void RefreshScreen(HWND hwnd){
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Double buffering setup
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBufferBmp = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

    // === RENDER WALLS ===
    SelectObject(hdcMem, wall);

    for (int y = 0; y < rect.bottom; y += leftWall.height) {
        for (int x = 0; x < rect.right; x += leftWall.width) {
            BitBlt(hdcBuffer, x, y, floorWall.width, leftWall.height, hdcMem, 0, 0, SRCCOPY);
        }
    }

    // === RENDER BACKGROUND ===
    SelectObject(hdcMem, background);

    int bgX = leftWall.width;
    int bgY = 0;
    int bgW = rect.right - leftWall.width - rightWall.width;
    int bgH = rect.bottom - floorWall.height;

    HPEN hpen = CreatePen(PS_SOLID, 3, RGB(255,255,255));
    HPEN oldPen  = (HPEN)SelectObject(hdcBuffer, hpen);
    Rectangle(hdcBuffer, bgX, bgY, bgX + bgW, bgY + bgH);
    SelectObject(hdcBuffer, oldPen);
    DeleteObject(hpen);

    StretchBlt(hdcBuffer, bgX, bgY, bgW, bgH, hdcMem, 0, 0, backgroundInfo.width, backgroundInfo.height, SRCCOPY);

    //traka s vremenom
    int barHeight = 25;
    int barGap = 8;
    int barY = bgY + bgH + barGap;
    int barX = bgX;
    int maxBarWidth = bgW;

    HBRUSH hDarkBrush = CreateSolidBrush(RGB(100,100,100));
    RECT barBgRect = {barX, barY, barX + maxBarWidth, barY + barHeight};
    FillRect(hdcBuffer, &barBgRect, hDarkBrush);
    DeleteObject(hDarkBrush);

    if (gameState.timeLeft < 0) gameState.timeLeft = 0;
    int currentWidth = (int)((gameState.timeLeft / maxTime) * maxBarWidth);

    HBRUSH hRedBrush = CreateSolidBrush(RGB(220, 0, 0));
    RECT barRect = {barX, barY, barX + currentWidth, barY + barHeight};
    FillRect(hdcBuffer, &barRect, hRedBrush);
    DeleteObject(hRedBrush);

    HBRUSH hLightRedBrush = CreateSolidBrush(RGB(255, 60, 60));
    RECT topBar = {barX, barY, barX + currentWidth, barY + 5};
    FillRect(hdcBuffer, &topBar, hLightRedBrush);
    DeleteObject(hLightRedBrush);

    HPEN hWhitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdcBuffer, hWhitePen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcBuffer, GetStockObject(NULL_BRUSH));
    Rectangle(hdcBuffer, barX, barY, barX + maxBarWidth, barY + barHeight);
    SelectObject(hdcBuffer, hOldPen);
    SelectObject(hdcBuffer, hOldBrush);
    DeleteObject(hWhitePen);


    //rendering balloons
     for (int i = 0; i < MAX_BALLOONS; i++) {
        if (balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &balloons[i]);
        }
    }

    // === RENDER HARPOON ===
    if (harpoon.isActive) {
        int visiblePart = std::min(harpoon.length, harpoon.height);
        int destY = rect.bottom - floorWall.height - visiblePart;

        SelectObject(hdcMem, arrowMask);
        StretchBlt(hdcBuffer, harpoon.x, destY, harpoon.width, visiblePart,
                   hdcMem, 0, 0, harpoon.width, visiblePart, SRCPAINT);

        SelectObject(hdcMem, arrow);
        StretchBlt(hdcBuffer, harpoon.x, destY, harpoon.width, visiblePart,
                   hdcMem, 0, 0, harpoon.width, visiblePart, SRCAND);
    }

    // === RENDER HERO ===
    int heroY = rect.bottom - floorWall.height - hero.height;
    int srcX = hero.currentFrame * hero.width;
    int srcY = hero.currentRow * hero.height;

    SelectObject(hdcMem, characterMask);
    BitBlt(hdcBuffer, hero.x, heroY, hero.width, hero.height, hdcMem, srcX, srcY, SRCAND);

    SelectObject(hdcMem, character);
    BitBlt(hdcBuffer, hero.x, heroY, hero.width, hero.height, hdcMem, srcX, srcY, SRCPAINT);

    if (!gameState.isLevelCleared && gameState.activeBalloonCount == 0 && gameState.timeLeft > 0 && !gameState.isGameOver) {
        gameState.isLevelCleared = true;
    }

    // WELL DONE TEKST //
    if (gameState.isLevelCleared) {
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(0, 255, 0));
    HFONT hBigFont = CreateFont(60, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                VARIABLE_PITCH, TEXT("Arial"));
    HFONT hOldFont = (HFONT)SelectObject(hdcBuffer, hBigFont);

    const char* msg = "WELL DONE!";
    SIZE size;
    GetTextExtentPoint32(hdcBuffer, msg, strlen(msg), &size);
    TextOut(hdcBuffer, (rect.right - size.cx) / 2, (rect.bottom - size.cy) / 2 - 100, msg, strlen(msg));

    SelectObject(hdcBuffer, hOldFont);
    DeleteObject(hBigFont);
    }
     // === GAME OVER TEKST ===
     else if (gameState.isGameOver) {
        SetBkMode(hdcBuffer, TRANSPARENT);
        SetTextColor(hdcBuffer, RGB(255, 0, 0));
        HFONT hBigFont = CreateFont(60, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                     CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                     VARIABLE_PITCH, TEXT("Arial"));
        HFONT hOldGameOverFont = (HFONT)SelectObject(hdcBuffer, hBigFont);

        const char* msg = "GAME OVER";
        SIZE size;
        GetTextExtentPoint32(hdcBuffer, msg, strlen(msg), &size);
        TextOut(hdcBuffer, (rect.right - size.cx) / 2, (rect.bottom - size.cy) / 2 - 100, msg, strlen(msg));

        SelectObject(hdcBuffer, hOldGameOverFont);
        DeleteObject(hBigFont);
    }

    // === COPY TO SCREEN ===
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuffer, 0, 0, SRCCOPY);

    // Cleanup
    SelectObject(hdcBuffer, oldBufferBmp);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcMem);
    DeleteDC(hdcBuffer);
    ReleaseDC(hwnd, hdc);
}

void RefreshSound(){
}

