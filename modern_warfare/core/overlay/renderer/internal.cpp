#include "internal.h"
#include <iostream>


namespace renderer {
	namespace internal {
		ID2D1Factory* d2d = nullptr;
		ID2D1HwndRenderTarget* render_target = nullptr;
		IDWriteFactory* factory = nullptr;

		bool init() {
			if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d))) {
				std::cout << "[-] failed to create render factory" << std::endl;		
				return false;
			}

			if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>((&factory))))) {
				std::cout << "[-] failed to create write factory" << std::endl;
				return false;
			}

			if (FAILED(d2d->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				D2D1::HwndRenderTargetProperties(hijack::hijacked_hwnd, hijack::window_size), &render_target))) {
				std::cout << "[-] failed to create render target" << std::endl;
				return false;
			}

			return true;
		}

		void shutdown( ) {
			d2d->Release( );
			factory->Release( );
			render_target->Release( );
		}
	}
}