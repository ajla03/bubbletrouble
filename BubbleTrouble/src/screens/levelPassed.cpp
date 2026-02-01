#include "resources.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"

void DrawLevelPassedScreen(HDC hdc, RECT rect){
    int w = rect.right;
    int h = rect.bottom;

    HDC overlayDC = CreateCompatibleDC(hdc);
    HBITMAP overlayBmp = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP oldBmp = (HBITMAP)SelectObject(overlayDC, overlayBmp);

    FillRect(overlayDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // alpha blend
    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 160;
    bf.AlphaFormat = 0;

    AlphaBlend(
        hdc,
        0, 0, w, h,
        overlayDC,
        0, 0, w, h,
        bf
    );

    SelectObject(hdc, gRes.hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    const char* text = "LEVEL PASSED!";

    SIZE sz;
    GetTextExtentPoint32(hdc, text, strlen(text), &sz);

    TextOut(
        hdc,
        (w - sz.cx) / 2,
        h / 4,
        text,
        strlen(text)
    );


    gGame.homeButtonInfo.x = w/2 - gGame.homeButtonInfo.width/2;
    gGame.homeButtonInfo.y = h/3;
    gGame.restartButtonInfo.x = gGame.homeButtonInfo.x - gGame.restartButtonInfo.width - 20;
    gGame.restartButtonInfo.y = h/3;
    gGame.nextButtonInfo.x = gGame.homeButtonInfo.x + gGame.homeButtonInfo.width + 20;
    gGame.nextButtonInfo.y = h/3;

    DrawButton(hdc, gRes.restartButton, gRes.restartButtonMask, gGame.restartButtonInfo);
    DrawButton(hdc, gRes.homeButton, gRes.homeButtonMask, gGame.homeButtonInfo);
    DrawButton(hdc, gRes.nextButton, gRes.nextButtonMask, gGame.nextButtonInfo);

    // cleanup
    SelectObject(overlayDC, oldBmp);
    DeleteObject(overlayBmp);
    DeleteDC(overlayDC);
}
