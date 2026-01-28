#include "resources.h"


void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, Button& button)
{
    HDC memDC = CreateCompatibleDC(hdc);

    SelectObject(memDC, mask);
    BitBlt(hdc, button.x, button.y, button.width, button.height, memDC, 0, 0, SRCPAINT);

    SelectObject(memDC, bmp);
    BitBlt(hdc, button.x, button.y, button.width, button.height, memDC, 0, 0, SRCAND);

 if (button.isHover)
    {
        HRGN rgn = CreateEllipticRgn(      // da napravimo sjenu samo na okruglom buttonu
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
        SelectObject(overlayDC, overlayBmp);

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

        SelectClipRgn(hdc, NULL);
        DeleteObject(rgn);
        DeleteObject(brush);
        DeleteObject(overlayBmp);
        DeleteDC(overlayDC);
    }

    DeleteDC(memDC);;
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


