#pragma once

#define GAME_NAME "Game_B"

#define GAME_RES_WIDTH      400
#define GAME_RES_HEIGHT     240
#define GAME_BITS_PER_PIXEL 32
#define GAME_CANVAS_SIZE   (GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BITS_PER_PIXEL / 8))

typedef struct GameBitmap {
	BITMAPINFO bitmapInfo;
	void* canvas;
} GameBitmap;

LRESULT CALLBACK MainWindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
void CreateMainGameWindowOrQuit(void);
BOOL GameIsAlreadyRunning(void);
void HandlePlayerInput(void);
void RenderFrameGraphics(void);
