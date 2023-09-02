#pragma once

#include <stdint.h>

#define GAME_NAME "Game_B"

#define GAME_RES_WIDTH      400
#define GAME_RES_HEIGHT     240
#define GAME_BITS_PER_PIXEL 32
#define GAME_CANVAS_SIZE   (GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BITS_PER_PIXEL / 8))

typedef struct GameBitmap {
	BITMAPINFO bitmapInfo;
	void* canvas;
} GameBitmap;

typedef struct Pixel32 {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} Pixel32;

LRESULT CALLBACK MainWindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
void CreateMainGameWindowOrQuit();
BOOL GameIsAlreadyRunning();
void HandlePlayerInput();
void RenderFrameGraphics();
