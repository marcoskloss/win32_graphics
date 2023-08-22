#pragma warning(push, 3)
#include <Windows.h>
#pragma warning(pop)

#include "main.h"

BOOL g_gameIsRunning;
HWND g_gameWindow;
GameBitmap g_backBuffer;

// SAL Annotations: https://learn.microsoft.com/en-us/cpp/c-runtime-library/sal-annotations?view=msvc-170

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ PSTR cmdLine, _In_ int cmdShow)
{
    if (GameIsAlreadyRunning()) {
        MessageBoxA(NULL, "Another instance of the game is already running!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    g_backBuffer.bitmapInfo.bmiHeader.biSize        = sizeof(g_backBuffer.bitmapInfo.bmiHeader);
    g_backBuffer.bitmapInfo.bmiHeader.biWidth       = GAME_RES_WIDTH;
    g_backBuffer.bitmapInfo.bmiHeader.biHeight      = GAME_RES_HEIGHT;
    g_backBuffer.bitmapInfo.bmiHeader.biBitCount    = GAME_BITS_PER_PIXEL;
    g_backBuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB; // 0 (no compression)
    g_backBuffer.bitmapInfo.bmiHeader.biPlanes      = 1;

    g_backBuffer.canvas = VirtualAlloc(NULL, GAME_CANVAS_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (!g_backBuffer.canvas) {
        MessageBoxA(NULL, "Failed to allocate memory for the backbuffer!", "Error", MB_ICONEXCLAMATION | MB_OK);
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

void CreateMainGameWindowOrQuit()
{
    WNDCLASSEXA windowClass = { 0 };
    HINSTANCE instance = GetModuleHandleA(NULL);

    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.lpfnWndProc = MainWindowProc;
    windowClass.hInstance = instance;
    windowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    windowClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
    windowClass.lpszMenuName = GAME_NAME "MenuName";
    windowClass.lpszClassName = GAME_NAME "WindowClassName";
    windowClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);

    if (!RegisterClassExA(&windowClass)) {
        MessageBoxA(NULL, "Failed to Register Window!", "Error", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }

    g_gameWindow = CreateWindowExA(
        WS_EX_CLIENTEDGE, windowClass.lpszClassName, GAME_NAME,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480, NULL, NULL,
        instance, NULL);

    if (!g_gameWindow) {
        MessageBoxA(NULL, "Failed to Create Window!", "Error", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }
}

BOOL GameIsAlreadyRunning()
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

void RenderFrameGraphics()
{
    HDC deviceContext = GetDC(g_gameWindow);

    StretchDIBits(
        deviceContext,
        0, 0, 100, 100,
        0, 0, 100, 100,
        g_backBuffer.canvas, &g_backBuffer.bitmapInfo,
        DIB_RGB_COLORS, SRCCOPY
    ); // move data from g_backBuffer.canvas to the deviceContext, streching or compressing if necessary

    ReleaseDC(g_gameWindow, deviceContext);
}