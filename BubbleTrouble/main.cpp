#include <windows.h>
#include <tchar.h>
#include <wingdi.h>
#include "resources.h"
#include "globals.h"
#include "game.h"

TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

void LoadCustomFontFromResource(HINSTANCE hInst){
    HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(IDR_MINI_FONT), RT_RCDATA);

    if(!hRes) return;

    DWORD size = SizeofResource(hInst, hRes);
    HGLOBAL hMem = LoadResource(hInst, hRes);
    void* data = LockResource(hMem);

    DWORD fontCount;
    hFontRes = AddFontMemResourceEx(
        data,
        size,
        NULL,
        &fontCount
    );

}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow){
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx(&wincl)) return 0;

    hwnd = CreateWindowEx(0, szClassName, _T("Igra"), WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
                          HWND_DESKTOP, NULL, hThisInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    LoadCustomFontFromResource(hThisInstance);

    LoadBitmaps(hwnd, hThisInstance);

    float targetFPS = 60.0f;
    DWORD frameTimeMs = (DWORD) (1000.0f / targetFPS);
    while (1){
        DWORD start = GetTickCount();
        if (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)){
            if (messages.message == WM_QUIT) break;
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        CheckInputs(hwnd);
        Update(hwnd);
        RefreshScreen(hwnd);
        RefreshSound();
        DWORD elapsed = GetTickCount() - start;
        if (elapsed < frameTimeMs)
            Sleep(frameTimeMs - elapsed);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
        case WM_SETCURSOR: {
         SetCursor(gameCursor);
         return 0 ;
        }
        case WM_CREATE:
        {
        hFont = CreateFont(
            48, 0, 0, 0,
            FW_BOLD,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY,
            VARIABLE_PITCH,
            TEXT("Kenney Mini Square")
            );
        }
          case WM_SIZE: {
            if (gameState.currentMode == GAME_MODE_MENU) {
                InitializeMenu(hwnd);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RefreshScreen(hwnd);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (gameState.currentMode == GAME_OVER || gameState.isLevelCleared == true) {
                int mx = LOWORD(lParam);
                int my = HIWORD(lParam);
                   if (mx >= restartButtonInfo.x &&
                       mx <= restartButtonInfo.x + restartButtonInfo.width &&
                       my >= restartButtonInfo.y &&
                       my <= restartButtonInfo.y + restartButtonInfo.height)
                    {
                        gameState.pendingRestart = true;
                        StartWallTransition(hwnd);
                        return 0;
                    }
                   if (mx >= homeButtonInfo.x &&
                        mx <= homeButtonInfo.x + homeButtonInfo.width &&
                        my >= homeButtonInfo.y &&
                        my <= homeButtonInfo.y + homeButtonInfo.height)
                    {
                        gameState.pendingHome = true;
                        StartWallTransition(hwnd);
                        return 0;
                     }
                    if(mx >= nextButtonInfo.x &&
                       mx <= nextButtonInfo.x + nextButtonInfo.width &&
                       my >= nextButtonInfo.y &&
                       my <= nextButtonInfo.y + nextButtonInfo.height &&
                       gameState.isLevelCleared == true){

                         MessageBox(hwnd, "Other levels arent implemented yet!", "Next level warning", MB_OK);
                         return 0;
                       }
            }else if (gameState.currentMode == GAME_MODE_MENU) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                HandleMenuClick(hwnd, x, y);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (gameState.currentMode == GAME_MODE_MENU) {
                HandleMenuMouseMove(hwnd, x, y);
            }else if(gameState.currentMode == GAME_OVER || gameState.isLevelCleared == true){
                CheckHover(homeButtonInfo, x, y);
                CheckHover(restartButtonInfo, x, y);
                CheckHover(nextButtonInfo, x, y);
            }
            return 0;
        }

        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                gameState.currentMode = GAME_MODE_MENU;
                InitializeMenu(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        case WM_DESTROY:
            RemoveFontMemResourceEx(hFontRes);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

