#pragma once
#include <d2d1.h>
#include <stdio.h>
#include <dwmapi.h> 
#pragma comment(lib, "Dwmapi.lib") 

namespace hijack {
	extern HWND hijacked_hwnd;
	extern RECT window_rect;
	extern D2D1_SIZE_U window_size;

	bool init();
}
