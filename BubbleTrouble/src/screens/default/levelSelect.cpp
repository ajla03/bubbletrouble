#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include <cmath>
#include <windows.h>
#include <stdio.h>

static void RenderBackground(HDC hdcBuffer, RECT rect);
static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet);
static void RenderTorches(HDC hdcBuffer, RECT sheet);
static void RenderBackButton(HDC hdcBuffer, RECT sheet);
static void RenderTitle(HDC hdcBuffer, RECT sheet);
static int s_hoveredLevel = -1;

void UpdateLevelSelectHover(HWND hwnd, int mx, int my) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    int padding = 120;
    int sheetWidth  = SHEET_W + padding * 2;
    int sheetHeight = SHEET_H;
    RECT sheet;
    sheet.left   = rect.right / 2 - sheetWidth / 2;
    sheet.right  = sheet.left + sheetWidth;
    sheet.top    = rect.bottom / 2 - sheetHeight / 2;
    sheet.bottom = sheet.top + sheetHeight;

    int btnW = 80, btnH = 80, gapX = 50, gapY = 40, cols = 4;
    int totalW = 4 * btnW + 3 * gapX;
    int startX = sheet.left + ((sheet.right - sheet.left) - totalW) / 2;
    int startY = sheet.top + 140;

    s_hoveredLevel = -1;
    for (int i = 0; i < 8; i++) {
        int row = i / cols, col = i % cols;
        int x = startX + col * (btnW + gapX);
        int y = startY + row * (btnH + gapY);
        if (mx >= x && mx <= x + btnW && my >= y && my <= y + btnH) {
            s_hoveredLevel = i;
            break;
        }
    }
}

void RenderLevelSelectScreen(HDC hdcBuffer, RECT rect) {
    RenderBackground(hdcBuffer, rect);

    RECT sheet;
    RenderTransparentSheet(hdcBuffer, rect, &sheet);

    RenderTorches(hdcBuffer, sheet);

    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFontTitle);
    RenderTitle(hdcBuffer, sheet);
    SelectObject(hdcBuffer, gRes.hFont);

    int currentUnlocked = gGame.gameState.isMultiplayer ? gGame.unlockedLevelMulti : gGame.unlockedLevelSingle;

    int btnW = 80;
    int btnH = 80;
    int gapX = 50;
    int gapY = 40;
    int cols = 4;

    int totalW = 4 * btnW + 3 * gapX;
    int startX = sheet.left + ((sheet.right - sheet.left) - totalW) / 2;
    int startY = sheet.top + 140;

    BITMAP bm;
    GetObject(gRes.settingsPlayer, sizeof(BITMAP), &bm);

    for (int i = 0; i < 8; i++) {
        int row = i / cols;
        int col = i % cols;

        int x = startX + col * (btnW + gapX);
        int y = startY + row * (btnH + gapY);

        bool isUnlocked = (i <= currentUnlocked);

        SelectObject(gRes.hdcMem, gRes.settingsPlayer);
        TransparentBlt(hdcBuffer, x, y, btnW, btnH, gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));

        SetBkMode(hdcBuffer, TRANSPARENT);

        if (isUnlocked) {
            if (s_hoveredLevel == i) {
                HPEN glowPen = CreatePen(PS_SOLID, 4, RGB(255, 140, 0));
                HGDIOBJ oldPen = SelectObject(hdcBuffer, glowPen);
                HGDIOBJ oldBrush = SelectObject(hdcBuffer, GetStockObject(HOLLOW_BRUSH));
                RoundRect(hdcBuffer, x, y - 2, x + btnW, y + btnH + 2, 20, 20);
                SelectObject(hdcBuffer, oldPen);
                SelectObject(hdcBuffer, oldBrush);
                DeleteObject(glowPen);
            }
            SetTextColor(hdcBuffer, RGB(255, 255, 255));
        } else {
            HDC maskDC = CreateCompatibleDC(hdcBuffer);
            HBITMAP maskBmp = CreateCompatibleBitmap(hdcBuffer, btnW, btnH);
            SelectObject(maskDC, maskBmp);
            RECT fillR = {0, 0, btnW, btnH};
            FillRect(maskDC, &fillR, (HBRUSH)GetStockObject(BLACK_BRUSH));
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 150, 0 };
            AlphaBlend(hdcBuffer, x, y, btnW, btnH, maskDC, 0, 0, btnW, btnH, bf);
            DeleteObject(maskBmp);
            DeleteDC(maskDC);
            SetTextColor(hdcBuffer, RGB(130, 130, 130));
        }

        RECT textRect = { x, y, x + btnW, y + btnH - 20 };
        char numStr[4];
        sprintf(numStr, "%d", i + 1);
        DrawTextA(hdcBuffer, numStr, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        int stars = gGame.gameState.isMultiplayer ? gGame.levelStarsMulti[i] : gGame.levelStarsSingle[i];
        if (isUnlocked && stars > 0) {
            BITMAP bmStar;
            GetObject(gRes.star, sizeof(BITMAP), &bmStar);

            int starW = 16;
            int starH = 16;
            int starGap = 2;
            int totalStarW = stars * starW + (stars - 1) * starGap;

            int sx = x + btnW / 2 - totalStarW / 2;
            int sy = y + btnH - starH - 4;

            HBITMAP oldMemBmp = (HBITMAP)SelectObject(gRes.hdcMem, gRes.star);
           for (int s = 0; s < stars; s++) {

                    TransparentBlt(
                        hdcBuffer,
                        sx + s * (starW + starGap), sy, starW, starH,
                        gRes.hdcMem,
                        0, 0, bmStar.bmWidth, bmStar.bmHeight,
                        RGB(255, 255, 255)
                    );
                }
            SelectObject(gRes.hdcMem, oldMemBmp);
        }
    }

    RenderBackButton(hdcBuffer, sheet);
}



static void RenderTitle(HDC hdcBuffer, RECT sheet) {
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(60, 60, 60));
    RECT textRect = { sheet.left, sheet.top + 30, sheet.right, sheet.top + 70 };
    DrawText(hdcBuffer, "SELECT LEVEL", -1, &textRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
}

static void RenderBackground(HDC hdcBuffer, RECT rect){
    SetStretchBltMode(hdcBuffer, HALFTONE);
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;
    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);
}

static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet) {
    int padding = 120;
    int sheetWidth  = SHEET_W + padding * 2;
    int sheetHeight = SHEET_H;

    outSheet->left   = rect.right / 2 - sheetWidth / 2;
    outSheet->right  = outSheet->left + sheetWidth;
    outSheet->top    = rect.bottom / 2 - sheetHeight / 2;
    outSheet->bottom = outSheet->top + sheetHeight;

    HBITMAP bmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
    HBITMAP hBmpOld = (HBITMAP) SelectObject(gRes.hdcMem, bmp);
    SetPixel(gRes.hdcMem, 0, 0, RGB(255, 255, 255));

    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 160, 0 };
    AlphaBlend(hdcBuffer, outSheet->left, outSheet->top,
               outSheet->right - outSheet->left, outSheet->bottom - outSheet->top,
               gRes.hdcMem, 0, 0, 1, 1, bf);

    SelectObject(gRes.hdcMem, hBmpOld);
    DeleteObject(bmp);

    // BORDER
    HPEN darkPen = CreatePen(PS_SOLID, 2, RGB(80,80,80));
    HPEN oldPen = (HPEN)SelectObject(hdcBuffer, darkPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdcBuffer, GetStockObject(NULL_BRUSH));
    Rectangle(hdcBuffer, outSheet->left - 2, outSheet->top - 2, outSheet->right + 2, outSheet->bottom + 2);

    HPEN lightPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    SelectObject(hdcBuffer, lightPen);
    Rectangle(hdcBuffer, outSheet->left, outSheet->top, outSheet->right, outSheet->bottom);

    SelectObject(hdcBuffer, oldPen);
    SelectObject(hdcBuffer, oldBrush);
    DeleteObject(darkPen); DeleteObject(lightPen);
}

static void RenderTorches(HDC hdcBuffer, RECT sheet) {
    float torchScale = 2.0f;
    int torchW = (int)(gGame.torchInfo.width * torchScale);
    int torchH = (int)(gGame.torchInfo.height * torchScale);
    int torchX1 = sheet.left / 2 - torchW / 2;
    int torchX2 = sheet.right + sheet.left / 2 - torchW / 2;
    int torchY = sheet.bottom / 2;
    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH, gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);
}

static void RenderBackButton(HDC hdcBuffer, RECT sheet) {
    int btnWidth = gGame.backButtonInfo.width;
    int btnHeight = gGame.backButtonInfo.height;
    int padding = 40;
    int x = sheet.left + padding;
    int y = sheet.bottom - btnHeight / 2 - padding;

    gGame.backButtonInfo.x = x;
    gGame.backButtonInfo.y = y;

    BITMAP bm;
    HBITMAP currentBitmap = gGame.backButtonInfo.isHover ? gRes.playerHover : gRes.settingsPlayer;
    GetObject(currentBitmap, sizeof(BITMAP), &bm);
    SelectObject(gRes.hdcMem, currentBitmap);
    TransparentBlt(hdcBuffer, x, y, btnWidth, btnHeight / 2, gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));

    if (gGame.backButtonInfo.isHover) {
        HPEN glowPen = CreatePen(PS_SOLID, 4, RGB(255, 140, 0));
        HGDIOBJ oldPen = SelectObject(hdcBuffer, glowPen);
        HGDIOBJ oldBrush = SelectObject(hdcBuffer, GetStockObject(HOLLOW_BRUSH));
        RoundRect(hdcBuffer, x, y - 2, x + btnWidth + 2, y + (btnHeight / 2) + 2, 20, 20);
        SelectObject(hdcBuffer, oldPen); SelectObject(hdcBuffer, oldBrush); DeleteObject(glowPen);
    }

    RECT textRect = { x, y, x + btnWidth, y + btnHeight / 2 };
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
