#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <algorithm>
#include <stdio.h>
#include <string>



void RenderStaticUI(HDC hdc, RECT rect) {
    if (CURRENT_LEVEL.hStaticCache == NULL  || CURRENT_LEVEL.staticRedraw) {

        if (CURRENT_LEVEL.hdcCache) DeleteDC(CURRENT_LEVEL.hdcCache);
        if (CURRENT_LEVEL.hStaticCache) DeleteObject(CURRENT_LEVEL.hStaticCache);

        CURRENT_LEVEL.hdcCache = CreateCompatibleDC(hdc);
        CURRENT_LEVEL.hStaticCache = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(CURRENT_LEVEL.hdcCache, CURRENT_LEVEL.hStaticCache);

        RenderWalls(CURRENT_LEVEL.hdcCache, rect);

        // === RENDER PLACEHOLDER ZA LEVEL ===
        SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);

        std::string levelText = "LEVEL " + std::to_string(gGame.currentLevel+1);
        HFONT oldFont = (HFONT)SelectObject(CURRENT_LEVEL.hdcCache, gRes.hFont);
        SetBkMode(CURRENT_LEVEL.hdcCache, TRANSPARENT);
        SetTextColor(CURRENT_LEVEL.hdcCache, RGB(60, 60, 60));

        SIZE textSize;
        GetTextExtentPoint32(CURRENT_LEVEL.hdcCache, levelText.c_str(), levelText.length(), &textSize);


        int barHeight = 25;
        int paddingX = 30;
        int boxW = textSize.cx + paddingX;
        int boxH = textSize.cy + paddingX;


        int placeholderX = (rect.right / 2) - (boxW / 2);
        int floorTop = rect.bottom - gGame.floorWall.height + barHeight;
        int placeholderY = floorTop + (gGame.floorWall.height - boxH) / 2;
        SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);

        SetStretchBltMode(CURRENT_LEVEL.hdcCache, COLORONCOLOR);
        StretchBlt(CURRENT_LEVEL.hdcCache, placeholderX, placeholderY, boxW, boxH, gRes.hdcMem, 0, 0,
                   gGame.levelPlaceholderInfo.width, gGame.levelPlaceholderInfo.height, SRCAND);

        SelectObject(gRes.hdcMem, gRes.levelPlaceholderBlack);

        StretchBlt(CURRENT_LEVEL.hdcCache, placeholderX, placeholderY, boxW, boxH, gRes.hdcMem, 0, 0,
                   gGame.levelPlaceholderInfo.width, gGame.levelPlaceholderInfo.height, SRCPAINT);

        TEXTMETRIC tm;
        GetTextMetrics(CURRENT_LEVEL.hdcCache, &tm);
        int textX = placeholderX + (boxW - textSize.cx) / 2;
        int textY = placeholderY + (boxH - tm.tmHeight) / 2;
        TextOut(CURRENT_LEVEL.hdcCache, textX, textY, levelText.c_str(), levelText.length());

        SelectObject(CURRENT_LEVEL.hdcCache, oldFont);


        // PLAYER PLACEHOLDER //
        int heartsTop = rect.bottom - gGame.floorWall.height + barHeight*2 + 10;
        int playerY = heartsTop + gGame.heartBgInfo.height + 10;

        gGame.playerHolderInfo.x = gGame.leftWall.width;
        gGame.playerHolderInfo.y = playerY;
        gGame.playerHolderInfo.width = boxW;
        gGame.playerHolderInfo.height= boxH;

        DrawPlayerPlaceholder(CURRENT_LEVEL.hdcCache, rect, "PLAYER 1", gGame.leftWall.width, playerY,
                             RGB(180, 0, 0), gGame.heartBgInfo.height + 6);


        CURRENT_LEVEL.staticRedraw = false;
    }

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, CURRENT_LEVEL.hdcCache, 0, 0, SRCCOPY);
}

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


void RenderDynamicGameUI(HDC hdc, RECT rect)
{

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    // PAUSE BUTTON //
    gGame.pauseButtonInfo.x = bgX + 15;
    DrawButton(hdc, gRes.pauseButton, gRes.pauseButtonMask, gGame.pauseButtonInfo);

    // SOUND BUTTON //
    gGame.soundButtonInfo.x = gGame.pauseButtonInfo.x + gGame.pauseButtonInfo.width + 15;
    DrawButton(hdc, gRes.soundButton, gRes.soundButtonMask, gGame.soundButtonInfo);

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
    DrawHeartsAndScore(hdc, rect, barHeight);

    std::string levelText = "LEVEL " + std::to_string(gGame.currentLevel+1);

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
    int placeholderX = (rect.right / 2) - (boxW / 2);

    // === RENDER BAKLJI ===
    float torchScale = 1.3f;

    int torchW = (int)(gGame.torchInfo.width  * torchScale);
    int torchH = (int)(gGame.torchInfo.height * torchScale);

    int torchX1 = gGame.leftWall.width/2 - torchW/2;
    int torchX2 = 3*gGame.leftWall.width/2 + bgW -  torchW/2;
    int torchY = bgH / 2 - torchH / 2;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    // Lijeva baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(
        hdc,
        torchX1, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCPAINT
    );
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(
        hdc,
        torchX1, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCAND
    );
    // Desna baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(
        hdc,
        torchX2, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCPAINT
    );
    SelectObject(gRes.hdcMem, gRes.torch);
   StretchBlt(
        hdc,
        torchX2, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCAND
    );
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
    else if (gGame.gameState.isGameOver || gGame.currentLevel >= 5 && gGame.gameState.isLevelCleared)
        DrawGameOverScreen(hdc, rect);
    else if(gGame.gameState.currentMode == GAME_MODE_PAUSE ){
        DrawPausedScreen(hdc, rect);
    }

}


void DrawHeartsAndScore(HDC hdc, RECT rect, int padding) {
    int startX = gGame.leftWall.width;
    int startY = rect.bottom - gGame.floorWall.height + padding + gGame.heartInfo.height;
    int gap = 30;

    int endX = 0 ;
    for (int i = 0; i < 5 ; i++) {
        int x = startX + i * (gGame.heartInfo.width + gap);
        int y = startY;
        endX = startX + gGame.heartBgInfo.width;

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

        // ========= SCORE ========= //
        int  gap = 10;
        x = gGame.playerHolderInfo.x + gGame.playerHolderInfo.width + gap;
        y = gGame.playerHolderInfo.y + 5  ;
        int boxW = x - endX;
        int boxH = gGame.playerHolderInfo.height - 8;
        SelectObject(gRes.hdcMem, gRes.scoreHolder);

        SetStretchBltMode(hdc, COLORONCOLOR);
        StretchBlt(
            hdc,
            x, y ,
            boxW, boxH,
            gRes.hdcMem,
            0, 0,
            gGame.scoreHolderInfo.width,
            gGame.scoreHolderInfo.height,
            SRCCOPY
        );
        HPEN hDarkPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hDarkPen);

       RoundRect(
            hdc,
            x -2,
            y -2,
            x + boxW + 2,
            y + boxH + 2,
            4, 4
        );

        SelectObject(hdc, hOldPen);
        DeleteObject(hDarkPen);

        HFONT oldFont = (HFONT)SelectObject(hdc, gRes.hFont);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(70, 70, 70));

        char valueText[32];
        sprintf(valueText, "%d", gGame.displayScore);
        SIZE textSize;
        GetTextExtentPoint32(
            hdc,
            valueText,
            lstrlen(valueText),
            &textSize
        );

        TextOut(
            hdc,
            x + padding/3,
            y + boxH/2 - textSize.cy/2,
            valueText,
            strlen(valueText)
        );

        SelectObject(hdc, oldFont);

        /*
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
        */

        SelectObject(gRes.hdcMem, oldBmp);

    }
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

    gGame.playerHolderInfo.x = x;
    gGame.playerHolderInfo.y = y;
    gGame.playerHolderInfo.width = boxW;
    gGame.playerHolderInfo.height = boxH;

    SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);
    SetStretchBltMode(hdc, COLORONCOLOR);
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
// ============ UI HELPER FUNCTIONS ============

// Moved from gameOver.cpp
void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, Button& button)
{
    // maska
    SelectObject(gRes.hdcMem, mask);
    BitBlt(hdc, button.x, button.y, button.width, button.height, gRes.hdcMem, 0, 0, SRCPAINT);

    // bitmap
    SelectObject(gRes.hdcMem, bmp);
    BitBlt(hdc, button.x, button.y, button.width, button.height, gRes.hdcMem, 0, 0, SRCAND);

    // SOUND OFF
    if (bmp == gRes.soundButton && !gGame.settingsState.soundState.soundEffectsOn)
    {
        HRGN rgn = CreateEllipticRgn(
            button.x,
            button.y,
            button.x + button.width,
            button.y + button.height
        );

        SelectClipRgn(hdc, rgn);

        BLENDFUNCTION bf{};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = 80;

        HDC overlayDC = CreateCompatibleDC(hdc);
        HBITMAP overlayBmp = CreateCompatibleBitmap(hdc, button.width, button.height);
        HBITMAP oldBmp = (HBITMAP)SelectObject(overlayDC, overlayBmp);

        RECT r{ 0, 0, button.width, button.height };
        FillRect(overlayDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

        AlphaBlend(
            hdc,
            button.x, button.y,
            button.width, button.height,
            overlayDC,
            0, 0,
            button.width, button.height,
            bf
        );

        SelectObject(overlayDC, oldBmp);
        SelectClipRgn(hdc, NULL);
        DeleteObject(rgn);
        DeleteObject(overlayBmp);
        DeleteDC(overlayDC);
    }

    // HOVER
    if (button.isHover)
    {
        HRGN rgn = CreateEllipticRgn(
            button.x,
            button.y,
            button.x + button.width,
            button.y + button.height
        );

        SelectClipRgn(hdc, rgn);

        HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
        BLENDFUNCTION bf{};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = 60;

        HDC overlayDC = CreateCompatibleDC(hdc);
        HBITMAP overlayBmp = CreateCompatibleBitmap(hdc, button.width, button.height);
        HBITMAP oldBmp = (HBITMAP)SelectObject(overlayDC, overlayBmp);

        RECT r = { 0, 0, button.width, button.height };
        FillRect(overlayDC, &r, brush);

        AlphaBlend(
            hdc,
            button.x, button.y,
            button.width, button.height,
            overlayDC,
            0, 0,
            button.width, button.height,
            bf
        );

        SelectObject(overlayDC, oldBmp);
        SelectClipRgn(hdc, NULL);
        DeleteObject(rgn);
        DeleteObject(brush);
        DeleteObject(overlayBmp);
        DeleteDC(overlayDC);
    }
}

// Moved from input.cpp
bool IsPointInButton(const Button& btn, int x, int y)
{
    return x >= btn.x &&
           x <= btn.x + btn.width &&
           y >= btn.y &&
           y <= btn.y + btn.height;
}

// Moved from update.cpp
void CheckHover(Button& button, int mx, int my)
{
    if (mx >= button.x && mx <= button.x + button.width &&
        my >= button.y && my <= button.y + button.height)
    {
        button.isHover = true;
    }
    else
    {
        button.isHover = false;
    }
}
