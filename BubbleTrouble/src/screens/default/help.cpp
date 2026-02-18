#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "constants.h"
#include <wingdi.h>
#include <cstdio>

// --- DEKLARACIJE ---
static void RenderBackground(HDC hdcBuffer, RECT rect);
static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet);
static void RenderHelpTitle(HDC hdcBuffer, RECT sheet);
static void RenderTorches(HDC hdcBuffer, RECT sheet);
static void RenderBackButton(HDC hdcBuffer, RECT sheet);
static void RenderHelpContent(HDC hdcBuffer, RECT sheet);
static void DrawInfoBox(HDC hdc, RECT rect, const char* title, HFONT titleFont);

// Pomoćna funkcija za crtanje dugmića
static void DrawKeyButton(HDC hdc, int x, int y, int w, int h, const char* label);

// --- GLAVNA FUNKCIJA ---
void RenderHelp(HDC hdcBuffer, RECT rect)
{
    RECT sheet;

    RenderBackground(hdcBuffer, rect);
    RenderTransparentSheet(hdcBuffer, rect, &sheet);
    RenderHelpTitle(hdcBuffer, sheet);

    // Sadržaj (Tekst i okviri)
    RenderHelpContent(hdcBuffer, sheet);

    RenderBackButton(hdcBuffer, sheet);
    RenderTorches(hdcBuffer, sheet);
}

// --- IMPLEMENTACIJA SADRŽAJA ---

static void DrawKeyButton(HDC hdc, int x, int y, int w, int h, const char* label) {
    BITMAP bm;
    GetObject(gRes.settingsPlayer, sizeof(BITMAP), &bm);

    HBITMAP old = (HBITMAP)SelectObject(gRes.hdcMem, gRes.settingsPlayer);

    // Crtamo pozadinu dugmeta
    TransparentBlt(hdc, x, y, w, h,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
                   RGB(255, 255, 255));

    SelectObject(gRes.hdcMem, old);

    // Ispisujemo tekst PREKO dugmeta (BIJELI)
    RECT r = {x, y, x + w, y + h};
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    HFONT oldF = (HFONT)SelectObject(hdc, gRes.hFontHelp);

    DrawText(hdc, label, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, oldF);
}

static void DrawInfoBox(HDC hdc, RECT rect, const char* title, HFONT titleFont) {
    // 1. Vanjski okvir
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(180, 180, 180));
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

    // 2. Naslovna traka okvira
    RECT titleRect = rect;
    titleRect.bottom = titleRect.top + 35;

    HBRUSH titleBg = CreateSolidBrush(RGB(60, 60, 60));
    FillRect(hdc, &titleRect, titleBg);
    DeleteObject(titleBg);

    // 3. Naslov okvira
    HFONT oldFont = (HFONT)SelectObject(hdc, gRes.hFontTable);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    DrawText(hdc, title, -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldFont);
    DeleteObject(hPen);
}

static void RenderHelpContent(HDC hdcBuffer, RECT sheet) {
    SetBkMode(hdcBuffer, TRANSPARENT);

    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFontTable);

    // --- UVODNI TEKST ---
    RECT textRect;
    textRect.left = sheet.left + 50;
    textRect.right = sheet.right - 50;
    textRect.top = sheet.top + 110;
    textRect.bottom = sheet.top + 180;

    SetTextColor(hdcBuffer, RGB(40, 40, 40));
    const char* helpMessage = "Bubble Trouble is an arcade game where you must destroy all bubbles before time runs out !";
    DrawText(hdcBuffer, helpMessage, -1, &textRect, DT_LEFT | DT_TOP | DT_WORDBREAK);


    // --- 3 OKVIRA ---
    int startY = sheet.top + 180;
    int boxHeight = 260;
    int totalWidth = (sheet.right - 50) - (sheet.left + 50);
    int gap = 20;

    int controlsWidth = (int)(totalWidth * 0.40);
    int otherWidth = (totalWidth - controlsWidth - (2 * gap)) / 2;

    RECT rControls = { sheet.left + 50, startY, sheet.left + 50 + controlsWidth, startY + boxHeight };
    RECT rPowerups = { rControls.right + gap, startY, rControls.right + gap + otherWidth, startY + boxHeight };
    RECT rTips     = { rPowerups.right + gap, startY, rPowerups.right + gap + otherWidth, startY + boxHeight };

    DrawInfoBox(hdcBuffer, rControls, "CONTROLS", gRes.hFontTable);
    DrawInfoBox(hdcBuffer, rPowerups, "POWER-UPS", gRes.hFontTable);
    DrawInfoBox(hdcBuffer, rTips, "TIPS", gRes.hFontTable);

    // --- SADRŽAJ UNUTAR OKVIRA ---

    // >>> CONTROLS <<<
    {
        int btnSize = 40;
        int btnSpaceW = 90;
        int rowHeight = 50;

        int startX = rControls.left + 15;
        int startY = rControls.top + 50;

        SelectObject(hdcBuffer, gRes.hFontTable);

        // 1. [<-] Move Left
        DrawKeyButton(hdcBuffer, startX, startY, btnSize, btnSize, "<-");

        // OBAVEZNO VRAĆAMO CRNU BOJU
        SetTextColor(hdcBuffer, RGB(20, 20, 20));
        RECT rText1 = { startX + btnSize + 15, startY, rControls.right, startY + btnSize };
        DrawText(hdcBuffer, "Move Left", -1, &rText1, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // 2. [->] Move Right
        int y2 = startY + rowHeight;
        DrawKeyButton(hdcBuffer, startX, y2, btnSize, btnSize, "->");

        // OBAVEZNO VRAĆAMO CRNU BOJU
        SetTextColor(hdcBuffer, RGB(20, 20, 20));
        RECT rText2 = { startX + btnSize + 15, y2, rControls.right, y2 + btnSize };
        DrawText(hdcBuffer, "Move Right", -1, &rText2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // 3. [SPACE] Fire Harpoon
        int y3 = y2 + rowHeight;
        DrawKeyButton(hdcBuffer, startX, y3, btnSpaceW, btnSize, "SPACE");

        // OBAVEZNO VRAĆAMO CRNU BOJU
        SetTextColor(hdcBuffer, RGB(20, 20, 20));
        RECT rText3 = { startX + btnSpaceW + 15, y3, rControls.right, y3 + btnSize };
        DrawText(hdcBuffer, "Fire Harpoon", -1, &rText3, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // Note
        SelectObject(hdcBuffer, gRes.hFontHelp);
        SetTextColor(hdcBuffer, RGB(80, 80, 80));
        RECT rNote = { rControls.left + 5, rControls.bottom - 40, rControls.right - 5, rControls.bottom };
        DrawText(hdcBuffer, "Controls can be customized in the settings menu.", -1, &rNote, DT_CENTER | DT_WORDBREAK);

        SelectObject(hdcBuffer, gRes.hFont);
    }

    // >>> POWER-UPS <<<
    {
        int iconSize = 30;
        int rowH = 65;
        int startIconY = rPowerups.top + 50;
        int textOffX = 45;

        auto DrawPowerRow = [&](HBITMAP hBmp, HBITMAP hMask, const char* title, const char* desc, int rowIdx) {
            int y = startIconY + (rowIdx * rowH);
            int x = rPowerups.left + 15;

            if(hBmp && hMask) {
                BITMAP bm;
                GetObject(hBmp, sizeof(BITMAP), &bm);
                HDC hdcMem = CreateCompatibleDC(hdcBuffer);

                int iconY = y + 5;

                HBITMAP old = (HBITMAP)SelectObject(hdcMem, hMask);
                StretchBlt(hdcBuffer, x, iconY, iconSize, iconSize, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

                SelectObject(hdcMem, hBmp);
                StretchBlt(hdcBuffer, x, iconY, iconSize, iconSize, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

                SelectObject(hdcMem, old);
                DeleteDC(hdcMem);
            }

            SelectObject(hdcBuffer, gRes.hFontTable);
            SetTextColor(hdcBuffer, RGB(20, 20, 20));
            RECT rTitle = { x + textOffX, y, rPowerups.right - 5, y + 20 };
            DrawText(hdcBuffer, title, -1, &rTitle, DT_LEFT | DT_TOP | DT_SINGLELINE);

            SelectObject(hdcBuffer, gRes.hFontTable);
            SetTextColor(hdcBuffer, RGB(60, 60, 60));
            RECT rDesc = { x + textOffX, y + 20, rPowerups.right - 5, y + 60 };
            DrawText(hdcBuffer, desc, -1, &rDesc, DT_LEFT | DT_TOP | DT_WORDBREAK);
        };
        SelectObject(hdcBuffer, gRes.hFontHelp);
        DrawPowerRow(gRes.lifePowerup,   gRes.lifePowerupMask,   "Extra Life",   "Grants an extra life.", 0);
        DrawPowerRow(gRes.timePowerup,   gRes.timePowerupMask,   "Time Extend",  "Adds extra time.", 1);
        DrawPowerRow(gRes.freezePowerup, gRes.freezePowerupMask, "Time Freeze",  "Temporarily stops all bubbles.", 2);
    }

    // >>> TIPS (NOVI TEKST I RASPRAVAK) <<<
    {
        SelectObject(hdcBuffer, gRes.hFontHelpSmall);
        SetTextColor(hdcBuffer, RGB(30, 30, 30));

        // Lista savjeta
        const char* tips[] = {
            "- Timer is your enemy!",
            "- Big bubbles = more points",
            "- Destroy big ones first",
            "- Don't get cornered",
            "- Use power-ups wisely",
            "- Practice makes perfect"
        };

        int numTips = 6;
        // Izračunavamo visinu dostupnog prostora
        int startY = rTips.top + 45;
        int endY = rTips.bottom - 20;
        int availableH = endY - startY;

        // Razmak između linija
        int lineStep = availableH / numTips;
        if (lineStep > 35) lineStep = 35; // Maksimalni razmak da ne bude prevelik

        for(int i = 0; i < numTips; i++) {
            int y = startY + (i * lineStep);
            RECT rLine = { rTips.left + 15, y, rTips.right - 5, y + lineStep };
            DrawText(hdcBuffer, tips[i], -1, &rLine, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }

    // Cleanup
    SelectObject(hdcBuffer, oldFont);
}

// --- OSTALE FUNKCIJE ---

static void RenderBackground(HDC hdcBuffer, RECT rect){
    SetStretchBltMode(hdcBuffer, HALFTONE);
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;
    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);
}

static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet) {
    // Malo manje proširenje (+160 umjesto +300)
    int helpSheetW = SHEET_W + 160;

    outSheet->left = rect.right / 2 - helpSheetW / 2;
    outSheet->right = outSheet->left + helpSheetW;

    outSheet->top = rect.bottom / 2 - SHEET_H / 2;
    outSheet->bottom = outSheet->top + SHEET_H;

    HBITMAP bmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
    HBITMAP hBmpOld = (HBITMAP) SelectObject(gRes.hdcMem, bmp);
    SetPixel(gRes.hdcMem, 0, 0, RGB(255, 255, 255));

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 180;
    bf.AlphaFormat = 0;
    AlphaBlend(hdcBuffer, outSheet->left, outSheet->top,
               outSheet->right - outSheet->left, outSheet->bottom - outSheet->top,
               gRes.hdcMem, 0, 0, 1, 1, bf);

    SelectObject(gRes.hdcMem, hBmpOld);
    DeleteObject(bmp);

    HPEN darkPen = CreatePen(PS_SOLID, 2, RGB(80,80,80));
    HPEN oldPen = (HPEN)SelectObject(hdcBuffer, darkPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdcBuffer, GetStockObject(NULL_BRUSH));
    Rectangle(hdcBuffer, outSheet->left - 2, outSheet->top - 2, outSheet->right + 2, outSheet->bottom + 2);
    HPEN lightPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    SelectObject(hdcBuffer, lightPen);
    Rectangle(hdcBuffer, outSheet->left, outSheet->top, outSheet->right, outSheet->bottom);
    SelectObject(hdcBuffer, oldPen);
    SelectObject(hdcBuffer, oldBrush);
    DeleteObject(darkPen);
    DeleteObject(lightPen);
}

static void RenderHelpTitle(HDC hdcBuffer, RECT sheet) {
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(60, 60, 60));

    LOGFONT lf; GetObject(gRes.hFont, sizeof(LOGFONT), &lf);
    lf.lfHeight = 50; lf.lfWidth = 0;
    HFONT hBigFont = CreateFontIndirect(&lf);
    HFONT hOldFont = (HFONT)SelectObject(hdcBuffer, hBigFont);

    RECT textRect = {sheet.left, sheet.top + 30, sheet.right, sheet.top + 100};
    DrawText(hdcBuffer, "HOW TO PLAY", -1, &textRect, DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOCLIP);

    int padding = 30;
    int lineY = textRect.top + lf.lfHeight;
    HPEN hLinePen = CreatePen(PS_SOLID, 4, RGB(190, 190, 190));
    HPEN hOldPen = (HPEN)SelectObject(hdcBuffer, hLinePen);
    MoveToEx(hdcBuffer, sheet.left + padding, lineY, NULL);
    LineTo(hdcBuffer, sheet.right - padding, lineY);

    SelectObject(hdcBuffer, hOldPen);
    SelectObject(hdcBuffer, hOldFont);
    DeleteObject(hLinePen);
    DeleteObject(hBigFont);
}

static void RenderBackButton(HDC hdcBuffer, RECT sheet) {
    int btnWidth = gGame.backButtonInfo.width;
    int btnHeight = gGame.backButtonInfo.height;

    int y = sheet.bottom - (btnHeight / 2) - 20;
    int padding = 40;
    int x = sheet.left + padding;

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
        int visualHeight = gGame.backButtonInfo.height/2;
        RoundRect(hdcBuffer,
                  x,
                  y - 2,
                  x + btnWidth + 2,
                  y + visualHeight + 2,
                  20, 20);

        SelectObject(hdcBuffer, oldPen);
        SelectObject(hdcBuffer, oldBrush);
        DeleteObject(glowPen);
    }

    RECT textRect = { x, y, x + btnWidth, y + btnHeight / 2 };
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    HFONT hFontOld = (HFONT) SelectObject(hdcBuffer, gRes.hFont);
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdcBuffer, hFontOld);
}

static void RenderTorches(HDC hdcBuffer, RECT sheet) {
    float torchScale = 2.0f;
    int torchW = (int)(gGame.torchInfo.width * torchScale);
    int torchH = (int)(gGame.torchInfo.height * torchScale);
    int torchX1 = sheet.left / 2 - torchW / 2;
    int torchX2 = sheet.right + sheet.left / 2 - torchW / 2;
    int torchY = sheet.bottom / 2;

    int srcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int srcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH, gRes.hdcMem, srcX, srcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH, gRes.hdcMem, srcX, srcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH, gRes.hdcMem, srcX, srcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH, gRes.hdcMem, srcX, srcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);
}
