#include "renderer.h"


namespace renderer {
	namespace fonts {
		IDWriteTextFormat* watermark_font = nullptr;
		IDWriteTextFormat* tahoma_font = nullptr;

		bool init( ) {
			if (FAILED(internal::factory->CreateTextFormat(L"Tahoma", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 22, L"en-us", &watermark_font))) {
				return false;
			}
			
			if (FAILED(internal::factory->CreateTextFormat(L"Tahoma", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13.0, L"en-us", &tahoma_font))) {
				return false;
			}

			return true;
		}

		void shutdown( ) {
			watermark_font->Release( );
			tahoma_font->Release( );
		}
	}
}