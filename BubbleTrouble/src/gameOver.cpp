#include "resources.h"


void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, StaticObject& info)
{
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, mask);

    BitBlt(hdc, info.x, info.y, info.width, info.height, memDC, 0, 0, SRCPAINT);

    SelectObject(memDC, bmp);
    BitBlt(hdc, info.x, info.y, info.width, info.height, memDC, 0, 0, SRCAND);

    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
}

void DrawGameOverScreen(HDC hdc, RECT rect)
{
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

    const char* text = "GAME OVER";
    SIZE sz;
    GetTextExtentPoint32(hdc, text, strlen(text), &sz);

    TextOut(
        hdc,
        (w - sz.cx) / 2,
        h / 4,
        text,
        strlen(text)
    );

    restartButtonInfo.x = w/2 - restartButtonInfo.width - 10;
    restartButtonInfo.y = h/3;
    homeButtonInfo.x = w/2 + 10;
    homeButtonInfo.y = h/3;

    DrawButton(hdc, restartButton, restartButtonMask, restartButtonInfo);
    DrawButton(hdc, homeButton, homeButtonMask, homeButtonInfo);

    // cleanup
    SelectObject(overlayDC, oldBmp);
    DeleteObject(overlayBmp);
    DeleteDC(overlayDC);
}


