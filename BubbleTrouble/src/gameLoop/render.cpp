#include "resources.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"
#include <windows.h>
#include <algorithm>

void DrawHearts(HDC, RECT, int);

void RefreshScreen(HWND hwnd){
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Double buffering setup
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBufferBmp = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

   if (gGame.gameState.currentMode == GAME_MODE_MENU) {
        RenderMenu(hdcBuffer, rect);
    }
   else if(gGame.gameState.currentMode == GAME_MODE_PLAYING || gGame.gameState.currentMode == GAME_OVER || gGame.gameState.isLevelCleared){

    // === RENDER WALLS ===
    SelectObject(hdcMem, gRes.wall);


    for (int y = 0; y < rect.bottom; y += gGame.leftWall.height) {
        for (int x = 0; x < rect.right; x += gGame.leftWall.width) {
            BitBlt(hdcBuffer, x, y, gGame.floorWall.width, gGame.leftWall.height, hdcMem, 0, 0, SRCCOPY);
        }
    }

    // === RENDER BACKGROUND ===
    SelectObject(hdcMem, gRes.background);

    int tileW = CURRENT_LEVEL.backgroundInfo.width;
    int tileH = CURRENT_LEVEL.backgroundInfo.height;

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    HPEN hpen = CreatePen(PS_SOLID, 3, RGB(255,255,255));
    HPEN oldPen  = (HPEN)SelectObject(hdcBuffer, hpen);
    //Rectangle(hdcBuffer, bgX, bgY, bgX + bgW, bgY + bgH);
    SelectObject(hdcBuffer, oldPen);
    DeleteObject(hpen);


    StretchBlt(hdcBuffer, bgX, bgY, bgW, bgH, hdcMem, 0, 0, CURRENT_LEVEL.backgroundInfo.width, CURRENT_LEVEL.backgroundInfo.height, SRCCOPY);

    /*
    for(int y = bgY; y < bgY + bgH; y += tileH) {
      for(int x = bgX; x < bgX + bgW; x += tileW) {
        int w = std::min(tileW, bgX + bgW - x);
        int h = std::min(tileH, bgY + bgH - y);

        BitBlt(hdcBuffer, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
      }
    }
    */

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

    if (CURRENT_LEVEL.timeLeft < 0) CURRENT_LEVEL.timeLeft = 0;
    int currentWidth = (int)((CURRENT_LEVEL.timeLeft / maxTime) * maxBarWidth);

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

    // ===== SRCA ===== //
    DrawHearts(hdcBuffer, rect, barHeight);

    // RENDER PLACEHOLDERA ZA LEVEL //
    SelectObject(hdcMem, gRes.levelPlaceholderWhite);

    const char *levelText = "LEVEL 1";

    // Font
    HFONT oldFont = (HFONT) SelectObject(hdcBuffer, gRes.hFont);
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(60, 60, 60));

    SIZE textSize;
    GetTextExtentPoint32(
        hdcBuffer,
        levelText,
        strlen(levelText),
        &textSize
    );

    // Padding oko teksta
    int paddingX = 40;
    int paddingY = 20;

    int boxW = textSize.cx + paddingX * 2;
    int boxH = gGame.levelPlaceholderInfo.height;

    int placeholderX = (rect.right / 2) - (boxW / 2);
    int placeholderY = rect.bottom - gGame.floorWall.height + 2*barHeight;

    SelectObject(hdcMem, gRes.levelPlaceholderWhite);
    StretchBlt(
        hdcBuffer,
        placeholderX, placeholderY,
        boxW, boxH,
        hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCAND
    );

    SelectObject(hdcMem, gRes.levelPlaceholderBlack);
    StretchBlt(
        hdcBuffer,
        placeholderX, placeholderY,
        boxW, boxH,
        hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCPAINT
    );

    TEXTMETRIC tm;
    GetTextMetrics(hdcBuffer, &tm);

    int textX = placeholderX + (boxW - textSize.cx) / 2;
    int textY = placeholderY + (boxH - tm.tmHeight) / 2;

    TextOut(hdcBuffer, textX, textY, levelText, strlen(levelText));

    SelectObject(hdcBuffer, oldFont);


    // RENDER BAKLJI //

    int torchGap = 10;
    int torchX1 = placeholderX - gGame.torchInfo.width - torchGap;
    int torchX2 = placeholderX + boxW + torchGap;
    int torchY = rect.bottom - gGame.floorWall.height + gGame.torchInfo.height/2 + barHeight;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;
    SelectObject(hdcMem, gRes.torchMask);
    BitBlt(hdcBuffer, torchX1, torchY, gGame.torchInfo.width, gGame.torchInfo.height, hdcMem, torchSrcX, torchSrcY, SRCPAINT);
    SelectObject(hdcMem, gRes.torch);
    BitBlt(hdcBuffer, torchX1, torchY, gGame.torchInfo.width, gGame.torchInfo.height, hdcMem, torchSrcX, torchSrcY, SRCAND);

    SelectObject(hdcMem, gRes.torchMask);
    BitBlt(hdcBuffer, torchX2, torchY, gGame.torchInfo.width, gGame.torchInfo.height, hdcMem, torchSrcX, torchSrcY, SRCPAINT);
    SelectObject(hdcMem, gRes.torch);
    BitBlt(hdcBuffer, torchX2, torchY, gGame.torchInfo.width, gGame.torchInfo.height, hdcMem, torchSrcX, torchSrcY, SRCAND);


    //rendering balloons
     for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }

    // === RENDER HARPOON ===
    if (gGame.harpoon.isActive) {
        int visiblePart = std::min(gGame.harpoon.length, gGame.harpoon.height);
        int destY = rect.bottom - gGame.floorWall.height - visiblePart;

        SelectObject(hdcMem, gRes.arrowMask);
        StretchBlt(hdcBuffer, gGame.harpoon.x, destY, gGame.harpoon.width, visiblePart,
                   hdcMem, 0, 0, gGame.harpoon.width, visiblePart, SRCPAINT);

        SelectObject(hdcMem, gRes.arrow);
        StretchBlt(hdcBuffer, gGame.harpoon.x, destY, gGame.harpoon.width, visiblePart,
                   hdcMem, 0, 0, gGame.harpoon.width, visiblePart, SRCAND);
    }

    // === RENDER HERO ===
    gGame.hero.y = rect.bottom - gGame.floorWall.height - gGame.hero.height;
    int srcX = gGame.hero.currentFrame * gGame.hero.width;
    int srcY = gGame.hero.currentRow * gGame.hero.height;

    SelectObject(hdcMem, gRes.characterMask);
    BitBlt(hdcBuffer, gGame.hero.x, gGame.hero.y, gGame.hero.width, gGame.hero.height, hdcMem, srcX, srcY, SRCAND);

    SelectObject(hdcMem, gRes.character);
    BitBlt(hdcBuffer, gGame.hero.x, gGame.hero.y , gGame.hero.width, gGame.hero.height, hdcMem, srcX, srcY, SRCPAINT);

    // WELL DONE TEKST //
    if (gGame.gameState.isLevelCleared){
        DrawLevelPassedScreen(hdcBuffer, rect);
    }
     // === GAME OVER TEKST ===
     else if (gGame.gameState.isGameOver) {
        DrawGameOverScreen(hdcBuffer, rect);
    }

   }

    DrawTransitionWalls(hdcBuffer, rect);

    // === COPY TO SCREEN ===
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuffer, 0, 0, SRCCOPY);

    // Cleanup
    SelectObject(hdcBuffer, oldBufferBmp);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcMem);
    DeleteDC(hdcBuffer);
    ReleaseDC(hwnd, hdc);
}


void DrawHearts(HDC hdc, RECT rect, int padding) {
    int startX = gGame.leftWall.width;
    int startY = rect.bottom - gGame.floorWall.height + padding + gGame.heartInfo.height;
    int gap = 5;

    HDC hdcMem = CreateCompatibleDC(hdc);

    for (int i = 0; i < 5 ; i++) {
        int x = startX + i * (gGame.heartInfo.width + gap);
        int y = startY;

        // === BACKGROUND  ===
        HBITMAP oldBmp = (HBITMAP)SelectObject(hdcMem, gRes.heartBkgMask);
        BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, hdcMem, 0, 0, SRCPAINT);
        SelectObject(hdcMem, gRes.heartBkg);
        BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, hdcMem, 0, 0, SRCAND);

        // === BORDER ===
        SelectObject(hdcMem, gRes.heartBorderMask);
        BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, hdcMem, 0, 0, SRCPAINT);
        SelectObject(hdcMem, gRes.heartBorder);
        BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, hdcMem, 0, 0, SRCAND);

        // === FILL  ===
        if (i < gGame.gameState.lives) {
            int srcX = gGame.hearts[i].currentFrame * gGame.heartInfo.width;

            SelectObject(hdcMem, gRes.heartMask);
            BitBlt(
                hdc,
                x,
                y,
                gGame.heartInfo.width,
                gGame.heartInfo.height,
                hdcMem,
                srcX,
                0,
                SRCPAINT
            );

            SelectObject(hdcMem, gRes.heart);
            BitBlt(
                hdc,
                x,
                y,
                gGame.heartInfo.width,
                gGame.heartInfo.height,
                hdcMem,
                srcX,
                0,
                SRCAND
            );
        }

        SelectObject(hdcMem, oldBmp);
    }

    DeleteDC(hdcMem);
}


void RefreshSound(){
}

