#include <windows.h>
#include <tchar.h>
#include <wingdi.h>
#include "resourceManager.h"
#include "gameContext.h"
#include "resources.h"
#include "database.h"
#include "game.h"
#include "loading.h"
#include <ctime>

TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow){

    srand((unsigned int)time(NULL));

    if (!InitDatabase()) {
        MessageBoxA(NULL, "Failed to initialize database!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

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

    DWORD style =
        WS_OVERLAPPED |        // osnovni prozor
        WS_CAPTION |           // title bar
        WS_SYSMENU |           // system menu
        WS_MINIMIZEBOX |       // minimize dozvoljen
        WS_MAXIMIZEBOX;        // fullscreen dozvoljen

    hwnd = CreateWindowEx(0, szClassName, _T("Bubble Trouble"), style,
                          20, 20, 1280, 720,
                          HWND_DESKTOP, NULL, hThisInstance, NULL);

    gRes.hIcon = (HICON)LoadImage(
        hThisInstance,
        MAKEINTRESOURCE(IDR_MAINICON),
        IMAGE_ICON,
        0, 0,
        LR_DEFAULTCOLOR
    );

    SendMessage(hwnd, WM_SETICON, ICON_BIG,   (LPARAM)gRes.hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)gRes.hIcon);

    gGame.gameState.currentMode = GAME_MODE_LOADING;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    LoadBitmaps(hwnd, hThisInstance);

    InitializeLoading(hwnd);


    while (gGame.gameState.currentMode == GAME_MODE_LOADING) {
        DWORD start = GetTickCount();

        if (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)){
            if (messages.message == WM_QUIT) return messages.wParam;
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }

        RefreshScreen(hwnd);

        DWORD elapsed = GetTickCount() - start;
        if (elapsed < 16) Sleep(16 - elapsed);
    }


    InitDefaultSettings();

    // Inicijalizacija zvuka
    mciSendString("close bgMusic", NULL, 0, NULL);
    mciSendString("open \"assets\\sounds\\music_loop.wav\" type waveaudio alias bgMusic", NULL, 0, NULL);
    mciSendString("play bgMusic notify", NULL, 0, hwnd);
    mciSendString("setaudio bgMusic volume to 200", NULL, 0, NULL);


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

        DWORD elapsed = GetTickCount() - start;
        if (elapsed < frameTimeMs)
            Sleep(frameTimeMs - elapsed);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
   static int prevClientW = 0;
   static int prevClientH = 0;
   static bool autoPaused = false;

    switch (message){
        case WM_SETCURSOR: {
         SetCursor(gRes.gameCursor);
         return 0;
        }

        case WM_CREATE:
        {

            // Kreiraj fontove
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

            gRes.hFontTitle = CreateFont(
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

            gRes.hFontTable = CreateFont(
                20, 0, 0, 0,
                FW_BOLD,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET,
                OUT_TT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                ANTIALIASED_QUALITY,
                VARIABLE_PITCH,
                TEXT("Kenney Mini Square")
            );



            // Inicijalizuj gRes.hdcBuffer i ostalo
            HDC hdc = GetDC(hwnd);
            gRes.Init(hdc, hwnd);
            ReleaseDC(hwnd, hdc);

            return 0;
        }
        case WM_SIZE: {

             SelectObject(gRes.hdcMem, gRes.loginPopup);
             BITMAP bm;
             GetObject(gRes.loginPopup, sizeof(BITMAP), &bm);

             RECT rect;
             GetClientRect(hwnd, &rect);

             int x = rect.right/2 - bm.bmWidth/2;
             int y = rect.bottom/2 - bm.bmHeight/2;

            if (wParam == SIZE_MINIMIZED) {
                 if(gGame.gameState.currentMode == GAME_MODE_PLAYING){
                    gGame.gameState.currentMode = GAME_MODE_PAUSE;
                    autoPaused = true;
                 }
                return 0;
            }

            if(wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED){
                if(gGame.gameState.currentMode == GAME_MODE_PAUSE && autoPaused){
                   gGame.gameState.currentMode = GAME_MODE_PLAYING;
                   autoPaused = false;
                }
            }

            int newW = LOWORD(lParam);
            int newH = HIWORD(lParam);

            if(newW <= 0 || newH <= 0)
                return 0;

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
            if(gGame.currentLevel == 3)
                RecalculateLevel4Layout(hwnd);
            if(CURRENT_LEVEL.ladder.width > 0)
                RecalculateLevel7Layout(hwnd);
            CURRENT_LEVEL.staticRedraw = true;

            HDC hdc = GetDC(hwnd);
            gRes.UpdateDC(hdc, hwnd);
            ReleaseDC(hwnd, hdc);

            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RefreshScreen(hwnd);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            HandleMouseClick(hwnd, LOWORD(lParam), HIWORD(lParam));
            return 0;
        }

        case WM_MOUSEMOVE: {
           HandleMouseMove(hwnd, LOWORD(lParam), HIWORD(lParam));
           return 0;
        }

        case WM_KEYDOWN: {
            HandleKeyDown(hwnd, wParam);
            return 0;
        }
        case WM_CHAR:{
            HandleCharInput(hwnd, wParam);
            return 0;
        }

        case MM_MCINOTIFY:
            if (wParam == MCI_NOTIFY_SUCCESSFUL) {
                mciSendString("play bgMusic from 0 notify", NULL, 0, hwnd);
            }
            return 0;

        case WM_DESTROY:
            mciSendString("close bgMusic", NULL, 0, NULL);
            RemoveFontMemResourceEx(gRes.hFontRes);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
