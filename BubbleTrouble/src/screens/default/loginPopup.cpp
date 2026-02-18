#include "resourceManager.h"
#include "gameContext.h"


static void RenderBackground(HDC hdcBuffer, RECT rect);
static void RenderLoginPopup(HDC hdcBuffer, RECT rect, HWND hwnd);
static void RenderNextButton(HDC hdcBuffer, RECT rect);
static void RenderPlayersName(HDC hdcBuffer, int x, int y);

void RenderLoginScreen(HDC hdcBuffer, RECT rect, HWND hwnd){

    // BACKGROUND
    RenderBackground(hdcBuffer, rect);

    // LOGIN POPUP
    RenderLoginPopup(hdcBuffer, rect, hwnd);

    // NEXT BUTTON
    RenderNextButton(hdcBuffer, rect);

}


static void RenderLoginPopup(HDC hdcBuffer, RECT rect, HWND hwnd){
 SelectObject(gRes.hdcMem, gRes.loginPopup);
 BITMAP bm;
 GetObject(gRes.loginPopup, sizeof(BITMAP), &bm);

 int x = rect.right/2 - bm.bmWidth/2;
 int y = rect.bottom/2 - bm.bmHeight/2;

  TransparentBlt(hdcBuffer,
                  x, y, bm.bmWidth, bm.bmHeight,
                  gRes.hdcMem,
                  0, 0,bm.bmWidth, bm.bmHeight,
                  RGB(255,255,255));

  RenderPlayersName(hdcBuffer, x + 90, y + 70);

}


static void RenderBackground(HDC hdcBuffer, RECT rect){
 SelectObject(gRes.hdcMem, gRes.loginBg);
 BITMAP bm;
 GetObject(gRes.loginBg, sizeof(BITMAP), &bm);

 TransparentBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0,bm.bmWidth, bm.bmHeight, RGB(255,255,255));

}

static void RenderNextButton(HDC hdcBuffer, RECT rect) {
    BITMAP bmPopup;
    GetObject(gRes.loginPopup, sizeof(BITMAP), &bmPopup);

    int xPopup = rect.right / 2 - bmPopup.bmWidth / 2;
    int yPopup = rect.bottom / 2 - bmPopup.bmHeight / 2;

    BITMAP bmButton;
    GetObject(gRes.loginButton, sizeof(BITMAP), &bmButton);

    int x = xPopup + (bmPopup.bmWidth / 2) - (bmButton.bmWidth / 2);

    int startOfBottomQuarter = yPopup + (bmPopup.bmHeight * 4 / 5);
    int bottomQuarterHeight = bmPopup.bmHeight / 5 - 50 ;
    int y = startOfBottomQuarter + (bottomQuarterHeight / 2) - (bmButton.bmHeight / 2);


    gGame.loginButtonInfo.x = x;
    gGame.loginButtonInfo.y = y;
    gGame.loginButtonInfo.width = bmButton.bmWidth;
    gGame.loginButtonInfo.height = bmButton.bmHeight/2;

    if(gGame.loginButtonInfo.isHover)
        SelectObject(gRes.hdcMem, gRes.loginButtonHover);
    else
        SelectObject(gRes.hdcMem, gRes.loginButton);

    TransparentBlt(hdcBuffer, x, y, bmButton.bmWidth, bmButton.bmHeight/2,
                   gRes.hdcMem, 0, 0, bmButton.bmWidth, bmButton.bmHeight,
                   RGB(255, 255, 255));

     if (gGame.loginButtonInfo.isHover) {
        HPEN glowPen = CreatePen(PS_SOLID, 4, RGB(255, 140, 0));
        HGDIOBJ oldPen = SelectObject(hdcBuffer, glowPen);
        HGDIOBJ oldBrush = SelectObject(hdcBuffer, GetStockObject(HOLLOW_BRUSH));
        int visualHeight = gGame.loginButtonInfo.height;
        RoundRect(hdcBuffer,
                  x,
                  y - 2,
                  x + gGame.loginButtonInfo.width + 2,
                  y + visualHeight + 2,
                  20, 20);

        SelectObject(hdcBuffer, oldPen);
        SelectObject(hdcBuffer, oldBrush);
        DeleteObject(glowPen);
    }
}

static void RenderPlayersName(HDC hdcBuffer, int x, int y)
{
    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFont);
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255,255,255));

    TextOutA(
        hdcBuffer,
        x,
        y,
        gGame.loginInput.text,
        gGame.loginInput.length
    );

    SIZE size;
    GetTextExtentPoint32A(hdcBuffer, gGame.loginInput.text, gGame.loginInput.length, &size);

    SelectObject(hdcBuffer, oldFont);

    // CARET
    if(gGame.loginInput.caretVisible)
    {
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255,165,0));
        HPEN hOld = (HPEN)SelectObject(hdcBuffer, hPen);

        int caretHeight = 28;
        MoveToEx(hdcBuffer, x + size.cx, y, NULL);
        LineTo(hdcBuffer, x + size.cx, y + caretHeight);
        SelectObject(hdcBuffer, hOld);
        DeleteObject(hPen);
    }
}



// INIT LOGIN INPUT
void InitLoginInput()
{
    gGame.loginInput.length = 0;
    gGame.loginInput.text[0] = '\0';
    gGame.loginInput.active = true;
    gGame.loginInput.caretVisible = true;
    gGame.loginInput.caretTimer = 0.0f;
}
