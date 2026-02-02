#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <algorithm>
#include <stdio.h>
#include <string>


void RenderWalls(HDC hdc, RECT rect){
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, NULL);
    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdc, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);

    // BIJELI OKVIR
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Rectangle(
        hdc,
        bgX - 1,
        bgY,
        bgX + bgW + 2,
        bgY + bgH + 3
    );

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);

}


void RenderGameUI(HDC hdc, RECT rect)
{

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;


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

    // Ograni�enje vremena na 0
    if (CURRENT_LEVEL.timeLeft < 0)
        CURRENT_LEVEL.timeLeft = 0;

    int currentWidth = (int)((CURRENT_LEVEL.timeLeft / maxTime) * maxBarWidth);

    HBRUSH hRedBrush = CreateSolidBrush(RGB(220, 0, 0));
    RECT barRect = { barX, barY, barX + currentWidth, barY + barHeight };
    FillRect(hdc, &barRect, hRedBrush);
    DeleteObject(hRedBrush);

    HBRUSH hLightRedBrush = CreateSolidBrush(RGB(255, 60, 60));
    RECT topBar = { barX, barY, barX + currentWidth, barY + 5 };
    FillRect(hdc, &topBar, hLightRedBrush);
    DeleteObject(hLightRedBrush);

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
    SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);

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

    int boxW = textSize.cx + paddingX;
    int boxH = gGame.floorWall.height / 2;

    int placeholderX = (rect.right / 2) - (boxW / 2);
    int floorTop = rect.bottom - gGame.floorWall.height + barHeight;

    int placeholderY =
    floorTop + (gGame.floorWall.height - boxH) / 2;
    // Maska
    SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);
    StretchBlt(
        hdc,
        placeholderX, placeholderY,
        boxW, boxH,
        gRes.hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCAND
    );

    // Bitmap
    SelectObject(gRes.hdcMem, gRes.levelPlaceholderBlack);
    StretchBlt(
        hdc,
        placeholderX, placeholderY,
        boxW, boxH,
        gRes.hdcMem,
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

    // PLAYER PLACEHOLDER //
    int heartsTop = rect.bottom - gGame.floorWall.height + barHeight*2 + 10;

    int playerY =heartsTop + gGame.heartBgInfo.height + 10;

    DrawPlayerPlaceholder(
        hdc,
        rect,
        "PLAYER 1",
        gGame.leftWall.width,
        playerY,
        RGB(180, 0, 0),
        gGame.heartBgInfo.height + 6
    );

    // === RENDER BAKLJI ===
    int torchGap = 10;
    int torchX1 = placeholderX - gGame.torchInfo.width - torchGap;
    int torchX2 = placeholderX + boxW + torchGap;
    int torchY = rect.bottom - gGame.floorWall.height + gGame.torchInfo.height / 2 + barHeight*2;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    // Lijeva baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    BitBlt(hdc, torchX1, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           gRes.hdcMem, torchSrcX, torchSrcY, SRCPAINT);

    SelectObject(gRes.hdcMem, gRes.torch);
    BitBlt(hdc, torchX1, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           gRes.hdcMem, torchSrcX, torchSrcY, SRCAND);

    // Desna baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    BitBlt(hdc, torchX2, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           gRes.hdcMem, torchSrcX, torchSrcY, SRCPAINT);

    SelectObject(gRes.hdcMem, gRes.torch);
    BitBlt(hdc, torchX2, torchY,
           gGame.torchInfo.width, gGame.torchInfo.height,
           gRes.hdcMem, torchSrcX, torchSrcY, SRCAND);

    // === HERO ===
    gGame.hero.y = rect.bottom - gGame.floorWall.height - gGame.hero.height;

    int srcX = gGame.hero.currentFrame * gGame.hero.width;
    int srcY = gGame.hero.currentRow * gGame.hero.height;

    SelectObject(gRes.hdcMem, gRes.characterMask);
    BitBlt(hdc, gGame.hero.x, gGame.hero.y,
           gGame.hero.width, gGame.hero.height,
           gRes.hdcMem, srcX, srcY, SRCAND);

    SelectObject(gRes.hdcMem, gRes.character);
    BitBlt(hdc, gGame.hero.x, gGame.hero.y,
           gGame.hero.width, gGame.hero.height,
           gRes.hdcMem, srcX, srcY, SRCPAINT);

    // === HARPOON ===
    if (gGame.harpoon.isActive) {
        int visible = std::min(gGame.harpoon.length, gGame.harpoon.height);
        int y = rect.bottom - gGame.floorWall.height - visible;

        SelectObject(gRes.hdcMem, gRes.arrowMask);
        StretchBlt(hdc, gGame.harpoon.x, y,
                   gGame.harpoon.width, visible,
                   gRes.hdcMem, 0, 0,
                   gGame.harpoon.width, visible, SRCPAINT);

        SelectObject(gRes.hdcMem, gRes.arrow);
        StretchBlt(hdc, gGame.harpoon.x, y,
                   gGame.harpoon.width, visible,
                   gRes.hdcMem, 0, 0,
                   gGame.harpoon.width, visible, SRCAND);
    }

    // === OVERLAY TEXT ===
    if (gGame.gameState.isLevelCleared && gGame.currentLevel < 4)
        DrawLevelPassedScreen(hdc, rect);
    else if (gGame.gameState.isGameOver || gGame.currentLevel >= 4 && gGame.gameState.isLevelCleared)
        DrawGameOverScreen(hdc, rect);

}


void DrawHearts(HDC hdc, RECT rect, int padding) {
    int startX = gGame.leftWall.width;
    int startY = rect.bottom - gGame.floorWall.height + padding + gGame.heartInfo.height;
    int gap = 5;

    for (int i = 0; i < 5 ; i++) {
        int x = startX + i * (gGame.heartInfo.width + gap);
        int y = startY;

        // === BACKGROUND  ===
        HBITMAP oldBmp = (HBITMAP)SelectObject(gRes.hdcMem, gRes.heartBkgMask);
        BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, gRes.hdcMem, 0, 0, SRCPAINT);
        SelectObject(gRes.hdcMem, gRes.heartBkg);
        BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, gRes.hdcMem, 0, 0, SRCAND);

        // === BORDER ===
        SelectObject(gRes.hdcMem, gRes.heartBorderMask);
        BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, gRes.hdcMem, 0, 0, SRCPAINT);
        SelectObject(gRes.hdcMem, gRes.heartBorder);
        BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, gRes.hdcMem, 0, 0, SRCAND);

        // === FILL  ===
        if (i < gGame.gameState.lives) {
            int srcX = gGame.hearts[i].currentFrame * gGame.heartInfo.width;

            SelectObject(gRes.hdcMem, gRes.heartMask);
            BitBlt(
                hdc,
                x,
                y,
                gGame.heartInfo.width,
                gGame.heartInfo.height,
                gRes.hdcMem,
                srcX,
                0,
                SRCPAINT
            );

            SelectObject(gRes.hdcMem, gRes.heart);
            BitBlt(
                hdc,
                x,
                y,
                gGame.heartInfo.width,
                gGame.heartInfo.height,
                gRes.hdcMem,
                srcX,
                0,
                SRCAND
            );
        }

        SelectObject(gRes.hdcMem, oldBmp);
    }

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

void DrawPlayerPlaceholder(
    HDC hdc,
    RECT rect,
    const std::string& text,
    int x,
    int y,
    COLORREF textColor,
    int boxHeight
) {
    HFONT oldFont = (HFONT)SelectObject(hdc, gRes.hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, textColor);

    SIZE textSize;
    GetTextExtentPoint32(
        hdc,
        text.c_str(),
        text.length(),
        &textSize
    );

    int paddingX = 30;
    int boxW = textSize.cx + paddingX;
    int boxH = boxHeight + 10;

    SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);
    StretchBlt(
        hdc,
        x, y,
        boxW, boxH,
        gRes.hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCAND
    );

    SelectObject(gRes.hdcMem, gRes.levelPlaceholderBlack);
    StretchBlt(
        hdc,
        x, y,
        boxW, boxH,
        gRes.hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCPAINT
    );

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);

    int textX = x + (boxW - textSize.cx) / 2;
    int textY = y + (boxH - tm.tmHeight) / 2;

    TextOut(
        hdc,
        textX,
        textY,
        text.c_str(),
        text.length()
    );

    SelectObject(hdc, oldFont);
}

