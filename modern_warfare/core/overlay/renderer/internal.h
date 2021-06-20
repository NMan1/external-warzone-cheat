#pragma once
#include "../hijack/hijack.h"

#include <stdio.h>
#include <dwmapi.h> 
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "Dwrite")

#pragma comment(lib, "Dwmapi.lib") 
#pragma comment(lib, "d2d1.lib")


namespace renderer {
	namespace internal {
		extern ID2D1Factory* d2d;
		extern ID2D1HwndRenderTarget* render_target;
		extern IDWriteFactory* factory;

		bool init( );
		void shutdown( );
	}
}
