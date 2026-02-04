#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"
#include "resources.h"


void StartWallTransition(HWND hwnd){
    RECT rect;
    GetClientRect(hwnd, &rect);

    gGame.animatedWalls.wallTopY = -gGame.animatedWall.height;
    gGame.animatedWalls.wallBottomY = rect.bottom;

    gGame.transitionState = TRANSITION_CLOSING;
    if(gGame.soundState.soundEffectsOn)
        PlaySound(MAKEINTRESOURCE(IDR_SLIDING_SOUND),
                                  GetModuleHandle(NULL),
                                  SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);

}


void DrawTransitionWalls(HDC hdc, RECT rect){

    if(gGame.transitionState == TRANSITION_NONE)
        return;

    int screenWidth = rect.right;

    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, NULL);

    SelectObject(gRes.hdcMem, gRes.wall2);
    StretchBlt(hdc, 0, gGame.animatedWalls.wallTopY, screenWidth, gGame.animatedWall.height, gRes.hdcMem, 0, 0, gGame.animatedWall.width, gGame.animatedWall.height, SRCCOPY);

    SelectObject(gRes.hdcMem, gRes.wall1);
    StretchBlt(hdc, 0, gGame.animatedWalls.wallBottomY, screenWidth, gGame.animatedWall.height, gRes.hdcMem, 0, 0, gGame.animatedWall.width, gGame.animatedWall.height, SRCCOPY);

}
