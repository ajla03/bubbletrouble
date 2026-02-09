#include "game.h"
#include "gameContext.h"
#include "database.h"
#include "resourceManager.h"

static void RenderBackground(HDC hdcBuffer, RECT rect);
static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet);
static void RenderDashboardTitle(HDC hdcBuffer, RECT sheet);
static void RenderBackButton(HDC hdcBuffer, RECT sheet);
static void RenderBestScore(HDC hdcBuffer, RECT sheet);
static void RenderCenteredText(HDC hdc, RECT rect, const char* scoreText, const char* labelText);

void RenderDashboard(HDC hdcBuffer, RECT rect){

    // POZADINA - sivi zid
    RenderBackground(hdcBuffer, rect);

    // SHEET
    RECT sheet;
    RenderTransparentSheet(hdcBuffer, rect,&sheet);

    // SCORE HOLDERS
    RenderBestScore(hdcBuffer, sheet);

    // TITLE
    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFont);
    RenderDashboardTitle(hdcBuffer, sheet);

    // BACK BUTTON //
    RenderBackButton(hdcBuffer, sheet);

    SelectObject(hdcBuffer, oldFont);

}

static void RenderBackground(HDC hdcBuffer, RECT rect){
    SetStretchBltMode(hdcBuffer, HALFTONE);
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;

    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);
}


static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet) {
    outSheet->left = rect.right / 2 - SHEET_W /2 - 30;
    outSheet->right = outSheet->left + SHEET_W + 60;
    outSheet->top = rect.bottom / 2 - SHEET_H / 2;
    outSheet->bottom = outSheet->top + SHEET_H;

    HBITMAP bmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
    HBITMAP hBmpOld = (HBITMAP) SelectObject(gRes.hdcMem, bmp);
    SetPixel(gRes.hdcMem, 0, 0, RGB(255, 255, 255));

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 160;
    bf.AlphaFormat = 0;
    AlphaBlend(hdcBuffer, outSheet->left, outSheet->top,
               outSheet->right - outSheet->left, outSheet->bottom - outSheet->top,
               gRes.hdcMem, 0, 0, 1, 1, bf);

    SelectObject(gRes.hdcMem, hBmpOld);
    DeleteObject(bmp);

    // SIVI BORDER //

    HPEN darkPen = CreatePen(PS_SOLID, 2, RGB(80,80,80));
    HPEN oldPen = (HPEN)SelectObject(hdcBuffer, darkPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdcBuffer, GetStockObject(NULL_BRUSH));

    Rectangle(
        hdcBuffer,
        outSheet->left - 2,
        outSheet->top - 2,
        outSheet->right + 2,
        outSheet->bottom + 2
    );

    HPEN lightPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    SelectObject(hdcBuffer, lightPen);

    Rectangle(
        hdcBuffer,
        outSheet->left,
        outSheet->top,
        outSheet->right,
        outSheet->bottom
    );

    SelectObject(hdcBuffer, oldPen);
    SelectObject(hdcBuffer, oldBrush);
    DeleteObject(darkPen);
    DeleteObject(lightPen);

}

static void RenderBestScore(HDC hdcBuffer, RECT sheet)
{
    BITMAP bmSingle, bmMulti;
    GetObject(gRes.singleScoreHolder, sizeof(BITMAP), &bmSingle);
    GetObject(gRes.multiScoreHolder,  sizeof(BITMAP), &bmMulti);

    int targetW = std::max(bmSingle.bmWidth,  bmMulti.bmWidth)*1.3;
    int targetH = std::max(bmSingle.bmHeight, bmMulti.bmHeight)*1.3;

    int spacing = 20;
    int totalWidth = targetW * 2 + spacing;

    int startX = sheet.left + (sheet.right - sheet.left - totalWidth) / 2;
    int startY = sheet.top  + (sheet.bottom - sheet.top - targetH) / 2 ;

    int xSingle = startX;
    int xMulti  = startX + targetW + spacing;

    // === GET SCORES ===
    HighScore singleScore = {};
    HighScore multiScore  = {};

    GetTopScores(&singleScore, 1, "SinglePlayer");
    GetTopScores(&multiScore,  1, "MultiPlayer");

    char singleText[32];
    char multiText[32];


    wsprintfA(singleText, "%d", singleScore.score);
    wsprintfA(multiText,  "%d", multiScore.score);

    // === DRAW SINGLE HOLDER ===
    SelectObject(gRes.hdcMem, gRes.singleScoreHolder);
    TransparentBlt(
        hdcBuffer,
        xSingle, startY,
        targetW, targetH,
        gRes.hdcMem,
        0, 0,
        bmSingle.bmWidth, bmSingle.bmHeight,
        RGB(255, 255, 255)
    );

    RECT singleRect = {
        xSingle,
        startY + spacing,
        xSingle + targetW,
        startY + targetH + spacing
    };

    RenderCenteredText(hdcBuffer, singleRect, singleText, "Single");

    // === DRAW MULTI HOLDER ===
    SelectObject(gRes.hdcMem, gRes.multiScoreHolder);
    TransparentBlt(
        hdcBuffer,
        xMulti, startY,
        targetW, targetH,
        gRes.hdcMem,
        0, 0,
        bmMulti.bmWidth, bmMulti.bmHeight,
        RGB(255, 255, 255)
    );

    RECT multiRect = {
        xMulti,
        startY + spacing,
        xMulti + targetW,
        startY + targetH + spacing
    };

    RenderCenteredText(hdcBuffer, multiRect, multiText, "Team");
}

void RenderCenteredText(HDC hdcBuffer, RECT rect, const char* scoreText, const char* labelText) {
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(184, 134, 11));

    TEXTMETRIC tmScore, tmLabel;

    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFont);
    GetTextMetrics(hdcBuffer, &tmLabel);
    SelectObject(hdcBuffer, gRes.hFontTitle);
    GetTextMetrics(hdcBuffer, &tmScore);

    int totalTextHeight = tmScore.tmHeight + tmLabel.tmHeight + 4;

    int startY = rect.top + (rect.bottom - rect.top - totalTextHeight) / 2;

    // SCORE
    RECT scoreRect = {
        rect.left,
        startY,
        rect.right,
        startY + tmScore.tmHeight
    };

    DrawTextA(
        hdcBuffer,
        scoreText,
        -1,
        &scoreRect,
        DT_CENTER | DT_SINGLELINE
    );
    SelectObject(hdcBuffer, gRes.hFont);
     RECT labelRect = {
        rect.left,
        startY + tmScore.tmHeight + 4,
        rect.right,
        startY + tmScore.tmHeight + 4 + tmLabel.tmHeight
    };

    DrawTextA(
        hdcBuffer,
        labelText,
        -1,
        &labelRect,
        DT_CENTER | DT_SINGLELINE
    );
    SelectObject(hdcBuffer, oldFont);

}

static void RenderDashboardTitle(HDC hdcBuffer, RECT sheet) {
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(60, 60, 60));

    HFONT oldFont = (HFONT) SelectObject(hdcBuffer, gRes.hFontTitle);

    RECT textRect;
    textRect.left = sheet.left;
    textRect.right = sheet.right;
    textRect.top = sheet.top + 20;
    textRect.bottom = sheet.top + 60;
    DrawText(hdcBuffer, "BEST SCORE BOARD", -1, &textRect, DT_CENTER | DT_TOP | DT_SINGLELINE);

    SelectObject(hdcBuffer, oldFont);

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
    HBITMAP currentBitmap;

    if (gGame.backButtonInfo.isHover)
        currentBitmap = gRes.playerHover;
    else
        currentBitmap = gRes.settingsPlayer;

    GetObject(currentBitmap, sizeof(BITMAP), &bm);
    SelectObject(gRes.hdcMem, currentBitmap);
    TransparentBlt(hdcBuffer, x, y, btnWidth, btnHeight / 2,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));

    RECT textRect = { x, y, x + btnWidth, y + btnHeight / 2 };
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
