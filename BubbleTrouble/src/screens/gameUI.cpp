#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <algorithm>
#include <stdio.h>
#include <string>

void RenderGameUI(HDC hdc, RECT rect)
{
    HDC hdcMem = CreateCompatibleDC(hdc);

    // === WALLS ===
    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

   // === LEFT WALL ===
    SelectObject(hdcMem, gRes.wall);
    for (int y = bgY; y < bgH; y += gGame.leftWall.height) {
        BitBlt(hdc,
               0, y,
               gGame.leftWall.width, gGame.leftWall.height,
               hdcMem,
               0, 0,
               SRCCOPY);
    }

    // === RIGHT WALL ===
    int rightX = bgX + bgW;
    for (int y = 0; y < bgH; y += gGame.rightWall.height) {
        BitBlt(hdc,
               rightX, y,
               gGame.rightWall.width, gGame.rightWall.height,
               hdcMem,
               0, 0,
               SRCCOPY);
    }

    // === FLOOR ===
    for (int x = 0; x < rect.right; x += gGame.floorWall.width) {
        BitBlt(hdc,
               x, bgH,
               gGame.floorWall.width, gGame.floorWall.height,
               hdcMem,
               0, 0,
               SRCCOPY);
    }

    // === TIME BAR ===
    int barHeight = 25;
    int barGap = 8;
    int barY = bgY + bgH + barGap;
    int barX = bgX;
    int maxBarWidth = bgW;

    // Pozadina trake
    HBRUSH hDarkBrush = CreateSolidBrush(RGB(100, 100, 100));
    RECT barBgRect = { barX, barY, barX + maxBarWidth, barY + barHeight };
    FillRect(hdc, &barBgRect, hDarkBrush);
    DeleteObject(hDarkBrush);

    // Ogranièenje vremena na 0
    if (CURRENT_LEVEL.timeLeft < 0)
        CURRENT_LEVEL.timeLeft = 0;

    int currentWidth = (int)((CURRENT_LEVEL.timeLeft / maxTime) * maxBarWidth);

    // Crven dio (g³ówna traka)
    HBRUSH hRedBrush = CreateSolidBrush(RGB(220, 0, 0));
    RECT barRect = { barX, barY, barX + currentWidth, barY + barHeight };
    FillRect(hdc, &barRect, hRedBrush);
    DeleteObject(hRedBrush);

    // Svetli vrh (highlight)
    HBRUSH hLightRedBrush = CreateSolidBrush(RGB(255, 60, 60));
    RECT topBar = { barX, barY, barX + currentWidth, barY + 5 };
    FillRect(hdc, &topBar, hLightRedBrush);
    DeleteObject(hLightRedBrush);

    // Ivica (okvir trake)
    HPEN hWhitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hWhitePen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, barX, barY, barX + maxBarWidth, barY + barHeight);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hWhitePen);

    // === HEARTS & SCORE ===
    DrawHearts(hdc, rect, barHeight);
    DrawScore(hdc, rect);

    // === RENDER PLACEHOLDER ZA LEVEL ===
    SelectObject(hdcMem, gRes.levelPlaceholderWhite);

    std::string levelText = "LEVEL " + std::to_string(gGame.currentLevel+1);
    // Font
    HFONT oldFont = (HFONT)SelectObject(hdc, gRes.hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(60, 60, 60));

    SIZE textSize;
    GetTextExtentPoint32(
        hdc,
        levelText.c_str(),
        levelText.length(),
        &textSize
    );

    // Padding oko teksta
    int paddingX = 40;
    int paddingY = 20;

    int boxW = textSize.cx + paddingX * 2;
    int boxH = gGame.levelPlaceholderInfo.height;

    int placeholderX = (rect.right / 2) - (boxW / 2);
    int placeholderY = rect.bottom - gGame.floorWall.height + 2 * barHeight;

    // Maska
    SelectObject(hdcMem, gRes.levelPlaceholderWhite);
    StretchBlt(
        hdc,
        placeholderX, placeholderY,
        boxW, boxH,
        hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCAND
    );

    // Bitmap
    SelectObject(hdcMem, gRes.levelPlaceholderBlack);
    StretchBlt(
        hdc,
        placeholderX, placeholderY,
        boxW, boxH,
        hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCPAINT
    );

    // Tekst centriran u placeholder
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);

    int textX = placeholderX + (boxW - textSize.cx) / 2;
    int textY = placeholderY + (boxH - tm.tmHeight) / 2;

    TextOut(hdc, textX, textY, levelText.c_str(), levelText.length());

    SelectObject(hdc, oldFont);

    // === RENDER BAKLJI ===
    int torchGap = 10;
    int torchX1 = placeholderX - gGame.torchInfo.width - torchGap;
    int torchX2 = placeholderX + boxW + torchGap;
    int torchY = rect.bottom - gGame.floorWall.height + gGame.torchInfo.height / 2 + barHeight;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    // Lijeva baklja
    SelectObject(hdcMem, gRes.torchMask);
    BitBlt(hdc, torchX1, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           hdcMem, torchSrcX, torchSrcY, SRCPAINT);

    SelectObject(hdcMem, gRes.torch);
    BitBlt(hdc, torchX1, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           hdcMem, torchSrcX, torchSrcY, SRCAND);

    // Desna baklja
    SelectObject(hdcMem, gRes.torchMask);
    BitBlt(hdc, torchX2, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           hdcMem, torchSrcX, torchSrcY, SRCPAINT);

    SelectObject(hdcMem, gRes.torch);
    BitBlt(hdc, torchX2, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           hdcMem, torchSrcX, torchSrcY, SRCAND);

    // === HERO ===
    gGame.hero.y = rect.bottom - gGame.floorWall.height - gGame.hero.height;

    int srcX = gGame.hero.currentFrame * gGame.hero.width;
    int srcY = gGame.hero.currentRow * gGame.hero.height;

    SelectObject(hdcMem, gRes.characterMask);
    BitBlt(hdc, gGame.hero.x, gGame.hero.y,
           gGame.hero.width, gGame.hero.height,
           hdcMem, srcX, srcY, SRCAND);

    SelectObject(hdcMem, gRes.character);
    BitBlt(hdc, gGame.hero.x, gGame.hero.y,
           gGame.hero.width, gGame.hero.height,
           hdcMem, srcX, srcY, SRCPAINT);

    // === HARPOON ===
    if (gGame.harpoon.isActive) {
        int visible = std::min(gGame.harpoon.length, gGame.harpoon.height);
        int y = rect.bottom - gGame.floorWall.height - visible;

        SelectObject(hdcMem, gRes.arrowMask);
        StretchBlt(hdc, gGame.harpoon.x, y,
                   gGame.harpoon.width, visible,
                   hdcMem, 0, 0,
                   gGame.harpoon.width, visible, SRCPAINT);

        SelectObject(hdcMem, gRes.arrow);
        StretchBlt(hdc, gGame.harpoon.x, y,
                   gGame.harpoon.width, visible,
                   hdcMem, 0, 0,
                   gGame.harpoon.width, visible, SRCAND);
    }

    // === OVERLAY TEXT ===
    if (gGame.gameState.isLevelCleared && gGame.currentLevel < 2)
        DrawLevelPassedScreen(hdc, rect);
    else if (gGame.gameState.isGameOver || gGame.currentLevel >=2 && gGame.gameState.isLevelCleared)
        DrawGameOverScreen(hdc, rect);

    DeleteDC(hdcMem);
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

void DrawScore(HDC hdc, RECT rect)
{
    HFONT oldFont = (HFONT)SelectObject(hdc, gRes.hFont);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    const char* label = "SCORE:";
    char valueText[32];
    sprintf(valueText, "%d", gGame.displayScore);

    SIZE labelSize;
    GetTextExtentPoint32(
        hdc,
        label,
        lstrlen(label),
        &labelSize
    );

    int padding = 30;

    int x = rect.right - gGame.rightWall.width - labelSize.cx - 120 - padding;
    int y = padding;

    TextOut(
        hdc,
        x,
        y,
        label,
        strlen(label)
    );

    TextOut(
        hdc,
        x + labelSize.cx + padding/3,
        y,
        valueText,
        strlen(valueText)
    );

    SelectObject(hdc, oldFont);
}


