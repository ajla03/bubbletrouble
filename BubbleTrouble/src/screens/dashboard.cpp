#include "game.h"
#include "gameContext.h"
#include "database.h"
#include "resourceManager.h"

static void RenderBackground(HDC hdcBuffer, RECT rect);
static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet);
static void RenderDashboardTitle(HDC hdcBuffer, RECT sheet);
static void RenderBackButton(HDC hdcBuffer, RECT sheet);
static void RenderBestScore(HDC hdcBuffer, RECT sheet);
static void RenderScoreTable(HDC hdcBuffer, int x, int y, const char* tableName, HBITMAP holderBmp);
static void RenderTorches(HDC hdcBuffer, RECT sheet);


void RenderDashboard(HDC hdcBuffer, RECT rect){

    // POZADINA - sivi zid
    RenderBackground(hdcBuffer, rect);

    // SHEET
    RECT sheet;
    RenderTransparentSheet(hdcBuffer, rect,&sheet);

    // TORCHES
    RenderTorches(hdcBuffer, sheet);

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
    BITMAP bm;
    GetObject(gRes.singleScoreHolder, sizeof(BITMAP), &bm);
    int tableW = bm.bmWidth;

    int startY = sheet.top + bm.bmHeight;

    int sheetInnerWidth = sheet.right - sheet.left;

    int spacing = (sheetInnerWidth - tableW * 2) / 3;

    int leftTableX  = sheet.left + spacing;
    int rightTableX = leftTableX + tableW + spacing;

    // SINGLE
    RenderScoreTable(
        hdcBuffer,
        leftTableX,
        startY,
        "SinglePlayer",
        gRes.singleScoreHolder
    );

    // MULTI
    RenderScoreTable(
        hdcBuffer,
        rightTableX,
        startY,
        "MultiPlayer",
        gRes.multiScoreHolder
    );
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


static void RenderScoreTable(
    HDC hdcBuffer,
    int x, int y,
    const char* tableName,
    HBITMAP holderBmp
){

    // === DRAW HOLDER ===
    BITMAP bm;
    GetObject(holderBmp, sizeof(BITMAP), &bm);
    SelectObject(gRes.hdcMem, holderBmp);

    TransparentBlt(
        hdcBuffer,
        x, y,
        bm.bmWidth, bm.bmHeight,
        gRes.hdcMem,
        0, 0,
        bm.bmWidth, bm.bmHeight,
        RGB(255,255,255)
    );

    // === DRAW BANNER ===
    HBITMAP bannerBmp =
        (strcmp(tableName, "SinglePlayer") == 0)
        ? gRes.singleplayerBanner
        : gRes.multiplayerBanner;

    BITMAP bannerBM;
    GetObject(bannerBmp, sizeof(BITMAP), &bannerBM);

    int bannerPadding = 12;

    int bannerX = x + (bm.bmWidth - bannerBM.bmWidth) / 2;
    int bannerY = y - bannerBM.bmHeight - bannerPadding;

    SelectObject(gRes.hdcMem, bannerBmp);

    TransparentBlt(
        hdcBuffer,
        bannerX,
        bannerY,
        bannerBM.bmWidth,
        bannerBM.bmHeight,
        gRes.hdcMem,
        0, 0,
        bannerBM.bmWidth,
        bannerBM.bmHeight,
        RGB(255,255,255)
    );

    // === GET TOP 5 ===
    HighScore scores[5] = {};
    GetTopScores(scores, 5, tableName);


   // === SCORE AREA ===
    int nameX1  = x + 107;
    int nameX2  = x + 252;

    int scoreX1 = x + 252;
    int scoreX2 = x + 369;

    int firstRowY = y + 32;
    int rowHeight = 37;

    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFontTable);
    SetBkMode(hdcBuffer, TRANSPARENT);

    for (int i = 0; i < 5; i++) {
        int rowY1 = firstRowY + i * rowHeight;
        int rowY2 = rowY1 + rowHeight;

        // NAME
        RECT nameRect = {
            nameX1,
            rowY1,
            nameX2,
            rowY2
        };

        SetTextColor(hdcBuffer, RGB(0,0,0));

        char displayName[50];

       if (scores[i].playerName[0] != '\0') {
            strncpy(displayName, scores[i].playerName, sizeof(displayName) - 1);
            displayName[sizeof(displayName) - 1] = '\0';
        } else {
            strcpy(displayName, "-");
        }

        DrawTextA(
            hdcBuffer,
            displayName,
            -1,
            &nameRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS
        );

        // SCORE
        char displayScore[32];
        if(scores[i].playerName[0] != '\0')
            wsprintfA(displayScore, "%d", scores[i].score);
        else
            strcpy(displayScore, "-");

        SetTextColor(hdcBuffer, RGB(140, 40, 45));

        RECT scoreRect = {
            scoreX1,
            rowY1,
            scoreX2,
            rowY2
        };

        DrawTextA(
            hdcBuffer,
            displayScore,
            -1,
            &scoreRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );

    }
    SelectObject(hdcBuffer, oldFont);
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

    // Prva baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

    // Druga baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);
}
