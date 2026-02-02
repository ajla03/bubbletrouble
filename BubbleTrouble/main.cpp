#include <windows.h>
#include <tchar.h>
#include <wingdi.h>
#include "resourceManager.h"
#include "gameContext.h"
#include "resources.h"
#include "game.h"

TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);


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
   static int prevClientW = 0;
   static int prevClientH = 0;

    switch (message){
        case WM_SETCURSOR: {
         SetCursor(gRes.gameCursor);

         return 0 ;
        }
        case WM_CREATE:
        {
        gRes.hFont = CreateFont(
            32, 0, 0, 0,
            FW_BOLD,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY,
            VARIABLE_PITCH,
            TEXT("Kenney Mini Square")
            );
            HDC hdc = GetDC(hwnd);
            gRes.Init(hdc, hwnd);
            ReleaseDC(hwnd, hdc);
        }
          case WM_SIZE: {
            int newW = LOWORD(lParam);
            int newH = HIWORD(lParam);


            UpdateLayout(
                prevClientW,
                prevClientH,
                newW,
                newH
            );

            prevClientW = newW;
            prevClientH = newH;

            if (gGame.gameState.currentMode == GAME_MODE_MENU) {
                InitializeMenu(hwnd);
            }
            if(gGame.currentLevel == 2)
                RecalculateLevel3Layout(hwnd);

            CURRENT_LEVEL.staticRedraw = true;

            HDC hdc = GetDC(hwnd);
            gRes.UpdateDC(hdc, hwnd);
            ReleaseDC(hwnd, hdc);

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
            if (gGame.gameState.currentMode == GAME_OVER || gGame.gameState.isLevelCleared == true) {
                int mx = LOWORD(lParam);
                int my = HIWORD(lParam);
                   if (mx >= gGame.restartButtonInfo.x &&
                       mx <= gGame.restartButtonInfo.x + gGame.restartButtonInfo.width &&
                       my >= gGame.restartButtonInfo.y &&
                       my <= gGame.restartButtonInfo.y + gGame.restartButtonInfo.height)
                    {
                        gGame.gameState.pendingRestart = true;
                        StartWallTransition(hwnd);
                        return 0;
                    }
                   if (mx >= gGame.homeButtonInfo.x &&
                        mx <= gGame.homeButtonInfo.x + gGame.homeButtonInfo.width &&
                        my >= gGame.homeButtonInfo.y &&
                        my <= gGame.homeButtonInfo.y + gGame.homeButtonInfo.height)
                    {
                        gGame.gameState.pendingHome = true;
                        StartWallTransition(hwnd);
                        return 0;
                     }
                    if(mx >= gGame.nextButtonInfo.x &&
                       mx <=gGame.nextButtonInfo.x + gGame.nextButtonInfo.width &&
                       my >= gGame.nextButtonInfo.y &&
                       my <= gGame.nextButtonInfo.y + gGame.nextButtonInfo.height &&
                       gGame.gameState.isLevelCleared == true){

                        gGame.gameState.pendingNextLevel = true;
                        StartWallTransition(hwnd);
                         return 0;
                       }
            }else if (gGame.gameState.currentMode == GAME_MODE_MENU) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                HandleMenuClick(hwnd, x, y);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (gGame.gameState.currentMode == GAME_MODE_MENU) {
                HandleMenuMouseMove(hwnd, x, y);
            }else if(gGame.gameState.currentMode == GAME_OVER || gGame.gameState.isLevelCleared == true){
                CheckHover(gGame.homeButtonInfo, x, y);
                CheckHover(gGame.restartButtonInfo, x, y);
                CheckHover(gGame.nextButtonInfo, x, y);
            }
            return 0;
        }

        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                gGame.gameState.currentMode = GAME_MODE_MENU;
                InitializeMenu(hwnd);
                //InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        case WM_DESTROY:
            RemoveFontMemResourceEx(gRes.hFontRes);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

