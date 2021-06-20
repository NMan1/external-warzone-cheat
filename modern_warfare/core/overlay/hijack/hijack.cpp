#include <iostream>
#include "hijack.h"

namespace hijack {
	HWND hijacked_hwnd{};
	RECT window_rect{};
	D2D1_SIZE_U window_size{};

	bool init() {
		hijacked_hwnd = FindWindowW(L"CEF-OSC-WIDGET", L"NVIDIA GeForce Overlay");
		if (!hijacked_hwnd) {
			std::cout << "[-] failed to find target window handle" << std::endl;
			return false;
		}

		auto apply_window_styles = []() {
			// style trasnperant
			SetWindowLongW(hijacked_hwnd, -20, static_cast<LONG_PTR>((static_cast<int>(GetWindowLongW(hijacked_hwnd, -20)) | 0x20)));

			// transparency
			MARGINS margin = { -1, -1, -1, -1 };
			DwmExtendFrameIntoClientArea(hijacked_hwnd, &margin);
			if (!SetLayeredWindowAttributes(hijacked_hwnd, 0x000000, 0xFF, 0x02)) {
				std::cout << "[-]" << " failed to set hijacked window attributes" << std::endl;
				return false;
			}

			SetWindowDisplayAffinity(hijacked_hwnd, WDA_EXCLUDEFROMCAPTURE);

			return true;
		};

		if (!apply_window_styles()) {
			return false;
		}

		ShowWindow(hijacked_hwnd, SW_SHOW);
		GetClientRect(hijacked_hwnd, &window_rect);
		window_size = D2D1::SizeU(window_rect.right - window_rect.left, window_rect.bottom - window_rect.top);

		return true;
	}
}
