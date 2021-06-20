#pragma once
#include "internal.h"
#include "../../utils/vectors.h"

namespace renderer {
	namespace colors {
		extern ID2D1SolidColorBrush* white_color;
		extern ID2D1SolidColorBrush* red_color;
		extern ID2D1SolidColorBrush* green_color;
		extern ID2D1SolidColorBrush* blue_color;
		extern ID2D1SolidColorBrush* black_color;

		bool init();
		void shutdown();
	}

	namespace fonts {
		extern IDWriteTextFormat* watermark_font;
		extern IDWriteTextFormat* tahoma_font;
		bool init();
		void shutdown();
	}

	namespace scene {
		void start(); 
		void begin();
		void end();
		void clear();
		void shutdown();

		void text(vec2_t pos, const wchar_t* text, ID2D1SolidColorBrush* color, IDWriteTextFormat* font);
		void box(int x, int y, int width, int height, int thick, ID2D1SolidColorBrush* color);
	}

	bool init();
	void shutdown();
}