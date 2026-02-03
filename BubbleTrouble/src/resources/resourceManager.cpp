#include "resources.h"
#include "resourceManager.h"

ResourceManager gRes = { 0 };

bool ResourceManager::LoadAll(HINSTANCE hInstance) {
    character     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HERO));
    characterMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HERO_MASK));
    arrow         = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_ARROW));
    arrowMask     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_ARROW_MASK));
    wall          = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_WALL));
    background    = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKGROUND));
    hBgLevel2     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKGROUND_LEVEL2));
    hBgLevel3     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKGROUND_LEVEL3));

    torch         = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TORCH));
    torchMask     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TORCH_MASK));
    levelPlaceholderWhite = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LEVEL_WHITE));
    levelPlaceholderBlack = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LEVEL_BLACK));
    scoreHolder = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SCORE_HOLDER));

    menuScreen        = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MENU_SCREEN));
    menuCharacter     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MENU_CHARACTER));
    menuCharacterMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MENU_CHARACTER_MASK));
    hButtonsHolder    = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BUTTONS_HOLDER));
    hButtonsHolderMask= LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BUTTONS_HOLDER_MASK));
    hMenuButton       = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MENU_BUTTON));
    hMenuButtonMask   = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MENU_BUTTON_MASK));

    restartButton     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_RESTART));
    restartButtonMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_RESTART_MASK));
    homeButton        = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOME));
    homeButtonMask    = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOME_MASK));
    nextButton        = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_NEXT));
    nextButtonMask    = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_NEXT_MASK));

    logo          = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LOGO));
    logoMask      = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LOGO_MASK));

    wall1 = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_WALL1));
    wall2 = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_WALL2));
    longWall= LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LONG_WALL));

    heart           = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HEART));
    heartMask       = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HEART_MASK));
    heartBorder     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HEART_BORDER));
    heartBorderMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HEART_BORDER_MASK));
    heartBkg        = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HEART_BKG));
    heartBkgMask    = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HEART_BKG_MASK));

    player1MenuChar     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PLAYER1_MENU));
    player1MenuCharMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PLAYER1_MENU_MASK));
    player2MenuChar     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PLAYER2_MENU));
    player2MenuCharMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PLAYER2_MENU_MASK));
    settingsMenuChar    = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SETTINGS_MENU));
    settingsMenuCharMask= LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SETTINGS_MENU_MASK));

    hYellowButton     = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_YELLOW_BUTTON));
    hYellowButtonMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_YELLOW_BUTTON_MASK));
    hGreenButton      = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_GREEN_BUTTON));
    hGreenButtonMask  = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_GREEN_BUTTON_MASK));
    hGreyButton       = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_GREY_BUTTON));
    hGreyButtonMask   = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_GREY_BUTTON_MASK));

    doorClosed = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_DOOR_CLOSED));
    doorClosedMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_DOOR_CLOSED_MASK));
    door  = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_DOOR));
    doorMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_DOOR_MASK));

    gameCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GAME_CURSOR));

    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(IDR_MINI_FONT), RT_RCDATA);

    DWORD size = SizeofResource(hInstance, hRes);
    HGLOBAL hMem = LoadResource(hInstance, hRes);
    void* data = LockResource(hMem);

    DWORD fontCount;
    hFontRes = AddFontMemResourceEx(
        data,
        size,
        NULL,
        &fontCount
    );

    if (!character || !background || !heart || !restartButton) {
        MessageBox(NULL, "Ne mogu da uèitam sve resurse!", "Greška", MB_ICONERROR);
        return false;
    }

    return true;
}

void ResourceManager::ReleaseAll() {
    HBITMAP* bmps = (HBITMAP*)this;
    for (int i = 0; i < sizeof(ResourceManager) / sizeof(HBITMAP); i++) {
        if (bmps[i]) DeleteObject(bmps[i]);

     //   SelectObject(hdcBuffer, oldBufferBmp);
       // DeleteObject(hbmBuffer);
        //DeleteDC(hdcBuffer);
    }

    if (hdcBuffer) {
        SelectObject(hdcBuffer, hOldBufferBmp);
        DeleteObject(hbmBuffer);
        DeleteDC(hdcBuffer);
    }

    if (hdcMem) DeleteDC(hdcMem);
}

void ResourceManager::Init(HDC hdc, HWND hwnd){
  hdcMem = CreateCompatibleDC(hdc);

  hdcBuffer = CreateCompatibleDC(hdc);
  RECT rect;
  GetClientRect(hwnd, &rect);
  hbmBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
  hOldBufferBmp = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);
}


void ResourceManager::UpdateDC(HDC hdc, HWND hwnd){

  if (hdcBuffer) {
        SelectObject(hdcBuffer, hOldBufferBmp);
        DeleteObject(hbmBuffer);
        DeleteDC(hdcBuffer);
    }
    if (hdcMem) {
        DeleteDC(hdcMem);
    }

    RECT rect;
    GetClientRect(hwnd, &rect);

    hdcMem = CreateCompatibleDC(hdc);
    hdcBuffer = CreateCompatibleDC(hdc);

    hbmBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    hOldBufferBmp = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);
}

