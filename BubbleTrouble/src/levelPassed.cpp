#include "resources.h"
#include "globals.h"
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

    SelectObject(hdc, hFont);
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


    homeButtonInfo.x = w/2 - homeButtonInfo.width/2;
    homeButtonInfo.y = h/3;
    restartButtonInfo.x = homeButtonInfo.x - restartButtonInfo.width - 20;
    restartButtonInfo.y = h/3;
    nextButtonInfo.x = homeButtonInfo.x + homeButtonInfo.width + 20;
    nextButtonInfo.y = h/3;

    DrawButton(hdc, restartButton, restartButtonMask, restartButtonInfo);
    DrawButton(hdc, homeButton, homeButtonMask, homeButtonInfo);
    DrawButton(hdc, nextButton, nextButtonMask, nextButtonInfo);

    // cleanup
    SelectObject(overlayDC, oldBmp);
    DeleteObject(overlayBmp);
    DeleteDC(overlayDC);
}
