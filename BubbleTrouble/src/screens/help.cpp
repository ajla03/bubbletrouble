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
    HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
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

    // --- FONTOVI ---
    HFONT hTextFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                PROOF_QUALITY, DEFAULT_PITCH, "Arial");

    HFONT hBoxTitleFont = CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    PROOF_QUALITY, DEFAULT_PITCH, "Arial");

    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, hTextFont);

    // --- UVODNI TEKST ---
    RECT textRect;
    textRect.left = sheet.left + 50;
    textRect.right = sheet.right - 50;
    textRect.top = sheet.top + 130;
    textRect.bottom = sheet.top + 200;

    SetTextColor(hdcBuffer, RGB(40, 40, 40));
    const char* helpMessage = "Bubble Trouble is an arcade game where you must \n destroy all bubbles before time runs out !";

    DrawText(hdcBuffer, helpMessage, -1, &textRect, DT_LEFT | DT_TOP | DT_WORDBREAK);


    // --- 3 OKVIRA ---
    int startY = sheet.top + 200;
    int boxHeight = 220;
    int totalWidth = (sheet.right - 50) - (sheet.left + 50);
    int gap = 20;
    int boxWidth = (totalWidth - (2 * gap)) / 3;

    RECT rControls = { sheet.left + 50, startY, sheet.left + 50 + boxWidth, startY + boxHeight };
    RECT rPowerups = { rControls.right + gap, startY, rControls.right + gap + boxWidth, startY + boxHeight };
    RECT rTips     = { rPowerups.right + gap, startY, rPowerups.right + gap + boxWidth, startY + boxHeight };

    DrawInfoBox(hdcBuffer, rControls, "CONTROLS", hBoxTitleFont);
    DrawInfoBox(hdcBuffer, rPowerups, "POWER-UPS", hBoxTitleFont);
    DrawInfoBox(hdcBuffer, rTips, "TIPS", hBoxTitleFont);

    // --- SADRŽAJ UNUTAR OKVIRA ---

    // CONTROLS
    RECT rContent = rControls;
    rContent.top += 45; rContent.left += 10; rContent.right -= 10;
    SetTextColor(hdcBuffer, RGB(20, 20, 20));
    DrawText(hdcBuffer, "Use ARROW keys to move\nLEFT and RIGHT.\n\nPress SPACE to shoot\nyour harpoon.",
             -1, &rContent, DT_CENTER | DT_WORDBREAK);

    // POWER-UPS (Ažurirano za transparentno crtanje s maskama)
    int iconSize = 30;
    int rowH = 45;
    int startIconY = rPowerups.top + 50;
    int textOffX = 40;

    // Lambda funkcija sada prima i Masku
    auto DrawPowerRow = [&](HBITMAP hBmp, HBITMAP hMask, const char* name, int rowIdx) {
        int y = startIconY + (rowIdx * rowH);
        int x = rPowerups.left + 20;

        if(hBmp && hMask) {
            BITMAP bm;
            GetObject(hBmp, sizeof(BITMAP), &bm);

            HDC hdcMem = CreateCompatibleDC(hdcBuffer);

            // 1. KORAK: MASKA (SRCAND)
            // Bijela pozadina maske ostavlja destinaciju netaknutom (AND 1 = 1),
            // Crni oblik maske "buši rupu" (AND 0 = 0).
            HBITMAP old = (HBITMAP)SelectObject(hdcMem, hMask);
            StretchBlt(hdcBuffer, x, y, iconSize, iconSize, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

            // 2. KORAK: SLIKA (SRCPAINT)
            // Crna pozadina slike ne mijenja destinaciju (OR 0 = x),
            // Boja slike popunjava rupu (OR color = color).
            SelectObject(hdcMem, hBmp);
            StretchBlt(hdcBuffer, x, y, iconSize, iconSize, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

            SelectObject(hdcMem, old);
            DeleteDC(hdcMem);
        }

        RECT rText = { x + textOffX, y, rPowerups.right - 10, y + iconSize };
        DrawText(hdcBuffer, name, -1, &rText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    };

    // Pozivamo funkciju s odgovarajućim maskama
    DrawPowerRow(gRes.lifePowerup,   gRes.lifePowerupMask,   "Extra Life",  0);
    DrawPowerRow(gRes.timePowerup,   gRes.timePowerupMask,   "Extra Time",  1);
    DrawPowerRow(gRes.freezePowerup, gRes.freezePowerupMask, "Freeze Time", 2);

    // TIPS
    rContent = rTips;
    rContent.top += 45; rContent.left += 10; rContent.right -= 10;
    DrawText(hdcBuffer, "- Don't rush!\n- Split big bubbles first.\n- Use walls for cover.\n- Watch the timer!",
             -1, &rContent, DT_LEFT | DT_WORDBREAK);

    // Cleanup
    SelectObject(hdcBuffer, oldFont);
    DeleteObject(hTextFont);
    DeleteObject(hBoxTitleFont);
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
    outSheet->left = rect.right / 2 - SHEET_W / 2;
    outSheet->right = outSheet->left + SHEET_W;
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

    RECT textRect = { x, y, x + btnWidth, y + btnHeight / 2 };
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
