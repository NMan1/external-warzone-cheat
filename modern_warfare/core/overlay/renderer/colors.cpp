#include "renderer.h"

namespace renderer {
	namespace colors {
		ID2D1SolidColorBrush* white_color = nullptr;
		ID2D1SolidColorBrush* red_color = nullptr;
		ID2D1SolidColorBrush* green_color = nullptr;
		ID2D1SolidColorBrush* blue_color = nullptr;
		ID2D1SolidColorBrush* black_color = nullptr;

		bool init( ) {
			internal::render_target->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::White ), &white_color );
			internal::render_target->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Red ), &red_color );
			internal::render_target->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Green ), &green_color );
			internal::render_target->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Blue ), &blue_color );
			internal::render_target->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black ), &black_color );
			return true;
		}

		void shutdown( ) {
			white_color->Release( );
			red_color->Release( );
			green_color->Release( );
			blue_color->Release( );
			black_color->Release( );
		}
	}
}
