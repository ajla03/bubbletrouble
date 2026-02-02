#pragma once
#include <windows.h>

struct ResourceManager{
    HBITMAP character, characterMask;
    HBITMAP arrow, arrowMask;
    HBITMAP wall, wall1, wall2;
    HBITMAP background;
    HBITMAP torch, torchMask;
    HBITMAP levelPlaceholderBlack, levelPlaceholderWhite;
    HBITMAP heart, heartMask;
    HBITMAP heartBkg, heartBkgMask;
    HBITMAP heartBorder, heartBorderMask;
    HBITMAP menuScreen, menuCharacter, menuCharacterMask;
    HBITMAP hButtonsHolder, hButtonsHolderMask;
    HBITMAP restartButton, restartButtonMask;
    HBITMAP homeButton, homeButtonMask;
    HBITMAP nextButton, nextButtonMask;
    HBITMAP hMenuButton, hMenuButtonMask;
    HBITMAP player1MenuChar, player1MenuCharMask;
    HBITMAP player2MenuChar, player2MenuCharMask;
    HBITMAP settingsMenuChar, settingsMenuCharMask;
    HBITMAP hYellowButton, hYellowButtonMask;
    HBITMAP hGreenButton, hGreenButtonMask;
    HBITMAP hGreyButton, hGreyButtonMask;
    HBITMAP hBgLevel2, hBgLevel3;
    HBITMAP longWall;
    HBITMAP doorClosed, doorClosedMask, door, doorMask;
    HCURSOR gameCursor;
    HBITMAP logo, logoMask;

    HFONT hFont;
    HANDLE hFontRes;

    bool LoadAll(HINSTANCE hInstance);
    void ReleaseAll();

};

extern ResourceManager gRes;
