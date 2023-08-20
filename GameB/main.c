#pragma warning(push, 3)
#include <Windows.h>
#pragma warning(pop)

#include "main.h"

BOOL g_gameIsRunning;
HWND g_gameWindow;
GameBitmap g_drawingSurface;

// SAL Annotations: https://learn.microsoft.com/en-us/cpp/c-runtime-library/sal-annotations?view=msvc-170

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ PSTR cmdLine, _In_ int cmdShow)
{
    if (GameIsAlreadyRunning()) {
        MessageBoxA(NULL, "Another instance of the game is already running!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    g_drawingSurface.bitmapInfo.bmiHeader.biSize        = sizeof(g_drawingSurface.bitmapInfo.bmiHeader);
    g_drawingSurface.bitmapInfo.bmiHeader.biWidth       = GAME_RES_WIDTH;
    g_drawingSurface.bitmapInfo.bmiHeader.biHeight      = GAME_RES_HEIGHT;
    g_drawingSurface.bitmapInfo.bmiHeader.biBitCount    = GAME_BITS_PER_PIXEL;
    g_drawingSurface.bitmapInfo.bmiHeader.biCompression = BI_RGB; // 0 (no compression)
    g_drawingSurface.bitmapInfo.bmiHeader.biPlanes      = 1;

    g_drawingSurface.canvas = VirtualAlloc(NULL, GAME_CANVAS_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (g_drawingSurface.canvas == NULL) {
        MessageBoxA(NULL, "Failed to allocate memory for drawing surface!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    CreateMainGameWindowOrQuit();

    MSG message = { 0 };
    g_gameIsRunning = TRUE;

    while (g_gameIsRunning) {
        while (PeekMessageA(&message, g_gameWindow, 0, 0, PM_REMOVE)) {
            DispatchMessageA(&message);
        }

        HandlePlayerInput();
        RenderFrameGraphics();
        // handle frame graphics

        Sleep(1); // revisit this one day
    }

    return 0;
}

LRESULT CALLBACK MainWindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CLOSE:
        g_gameIsRunning = FALSE;
        PostQuitMessage(0);
        break;

    case WM_DESTROY:
        g_gameIsRunning = FALSE;
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(windowHandle, message, wParam, lParam);
    }

    return 0;
}

void CreateMainGameWindowOrQuit(void)
{
    WNDCLASSEXA windowClass = { 0 };
    HINSTANCE instance = GetModuleHandleA(NULL);

    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.style = 0;
    windowClass.lpfnWndProc = MainWindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = instance;
    windowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = GAME_NAME "MenuName";
    windowClass.lpszClassName = GAME_NAME "WindowClassName";
    windowClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);

    if (RegisterClassExA(&windowClass) == 0) {
        MessageBoxA(NULL, "Failed to Register Window!", "Error", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }

    g_gameWindow = CreateWindowExA(
        WS_EX_CLIENTEDGE, windowClass.lpszClassName, GAME_NAME,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480, NULL, NULL,
        instance, NULL);

    if (g_gameWindow == NULL) {
        MessageBoxA(NULL, "Failed to Create Window!", "Error", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }
}

BOOL GameIsAlreadyRunning(void)
{
    HANDLE mutex = CreateMutexA(NULL, FALSE, GAME_NAME "Mutex");
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

void HandlePlayerInput()
{
    short escapeKeyIsDown = GetAsyncKeyState(VK_ESCAPE);

    if (escapeKeyIsDown) {
        SendMessageA(g_gameWindow, WM_CLOSE, 0, 0);
    }
}

void RenderFrameGraphics(void)
{

}