#include "game.h"
#include "resourceManager.h"
#include <wingdi.h>

void RenderSettings(HDC hdcBuffer, RECT rect)
{
    HDC overlayDC = CreateCompatibleDC(hdcBuffer);
    HBITMAP overlayBmp = CreateCompatibleBitmap(hdcBuffer, rect.right, rect.bottom);
    HBITMAP oldBmp = (HBITMAP)SelectObject(overlayDC, overlayBmp);

    FillRect(overlayDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // alpha blend
    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 160;
    bf.AlphaFormat = 0;

    AlphaBlend(
        hdcBuffer,
        0, 0, rect.right, rect.bottom,
        overlayDC,
        0, 0, rect.right, rect.bottom,
        bf
    );

    SelectObject(overlayDC, oldBmp);
    DeleteObject(overlayBmp);
    DeleteDC(overlayDC);

    // === POPUP DIMENZIJE ===
    int popupWidth  = rect.right  / 3;
    int popupHeight = rect.bottom / 1.5;

    int popupX = (rect.right  - popupWidth)  / 2;
    int popupY = (rect.bottom - popupHeight) / 2;

    // === SJENA (shadow) ===
    HBRUSH hShadowBrush = CreateSolidBrush(RGB(120, 120, 120));
    HBRUSH oldShadow = (HBRUSH)SelectObject(hdcBuffer, hShadowBrush);

    RoundRect(
        hdcBuffer,
        popupX + 6, popupY + 6,
        popupX + popupWidth + 6,
        popupY + popupHeight + 6,
        25, 25
    );

    SelectObject(hdcBuffer, oldShadow);
    DeleteObject(hShadowBrush);

    // === GLAVNI POPUP ===
    HBRUSH hPopupBrush = CreateSolidBrush(RGB(230, 230, 230)); // svijetlo siva
    HPEN hPopupPen = CreatePen(PS_SOLID, 2, RGB(180, 180, 180));

    HBRUSH oldPopupBrush = (HBRUSH)SelectObject(hdcBuffer, hPopupBrush);
    HPEN oldPen = (HPEN)SelectObject(hdcBuffer, hPopupPen);

    RoundRect(
        hdcBuffer,
        popupX, popupY,
        popupX + popupWidth,
        popupY + popupHeight,
        25, 25
    );

    SelectObject(hdcBuffer, oldPopupBrush);
    SelectObject(hdcBuffer, oldPen);
    DeleteObject(hPopupBrush);
    DeleteObject(hPopupPen);

    // === NASLOV ===
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(60, 60, 60));

    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFont);

    const char* title = "SETTINGS";
    SIZE textSize;
    GetTextExtentPoint32(hdcBuffer, title, (int)strlen(title), &textSize);

    int textX = popupX + (popupWidth - textSize.cx) / 2;
    int textY = popupY + 20;

    TextOut(hdcBuffer, textX, textY, title, (int)strlen(title));

    SelectObject(hdcBuffer, oldFont);
}
