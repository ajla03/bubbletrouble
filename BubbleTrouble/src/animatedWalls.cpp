#include "globals.h"
#include "resourceManager.h"
#include "game.h"

void StartWallTransition(HWND hwnd){
    RECT rect;
    GetClientRect(hwnd, &rect);

    animatedWalls.wallTopY = -animatedWall.height;
    animatedWalls.wallBottomY = rect.bottom;

    transitionState = TRANSITION_CLOSING;

}


void DrawTransitionWalls(HDC hdc, RECT rect){

    if(transitionState == TRANSITION_NONE)
        return;

    HDC memDC = CreateCompatibleDC(hdc);
    int screenWidth = rect.right;

    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, NULL);

    SelectObject(memDC, gRes.wall2);
    StretchBlt(hdc, 0, animatedWalls.wallTopY, screenWidth, animatedWall.height, memDC, 0, 0, animatedWall.width, animatedWall.height, SRCCOPY);

    SelectObject(memDC, gRes.wall1);
    StretchBlt(hdc, 0, animatedWalls.wallBottomY, screenWidth, animatedWall.height, memDC, 0, 0, animatedWall.width, animatedWall.height, SRCCOPY);

    DeleteDC(memDC);
}
