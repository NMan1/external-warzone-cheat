#include <iostream>
#include "driver/driver.h"
#include "game/offsets.h"
#include "overlay/hijack/hijack.h"
#include "overlay/renderer/renderer.h"
#include <string>

#define ERROR(msg) std::cout << "[-] " << msg << std::endl; std::cin.get(); exit(EXIT_FAILURE); 
#define ASSERT(cond, msg) if (!cond) { ERROR(msg) }

int main() {
	sdk::process_id = driver::get_process_id("ModernWarfare.exe");
	ASSERT(sdk::process_id, "failed to find pid");

	sdk::module_base = driver::get_module_base_address("ModernWarfare.exe");
	ASSERT(sdk::module_base, "failed to module base");

	sdk::peb = driver::get_peb();
	ASSERT(sdk::peb, "failed to find peb");

	sdk::set_game_hwnd();
	ASSERT(sdk::hwnd, "failed to find window handle");

	ASSERT(hijack::init(), "failed to hijack nvidia overlay");
	ASSERT(renderer::init(), "failed to initlize renderer");

	sdk::client_info = decryption::get_client_info();
	ASSERT(sdk::client_info, "failed to find client_info");

	sdk::client_info_base = decryption::get_client_info_base();
	ASSERT(sdk::client_info_base, "failed to find client_info_base");

	auto ref_def_ptr = decryption::get_ref_def();
	ASSERT(ref_def_ptr, "failed to find ref_def_ptr");

	std::cout << "[-] log" << std::endl;
	std::cout << "	[~] pid:  " << std::hex << sdk::process_id << std::endl;
	std::cout << "	[~] base: " << std::hex << sdk::module_base << std::endl;
	std::cout << "	[~] peb:  " << std::hex << sdk::peb << std::endl;
	std::cout << "	[~] hwnd: " << std::hex << sdk::hwnd << std::endl;
	std::cout << "	[~] client_info:      " << std::hex << sdk::client_info << std::endl;
	std::cout << "	[~] client_info_base: " << std::hex << sdk::client_info_base << std::endl;

	sdk::ref_def = driver::read<sdk::ref_def_t>(ref_def_ptr);

	while (!GetAsyncKeyState(VK_DELETE)) {
		renderer::scene::start();

		renderer::scene::text(vec2_t(sdk::ref_def.width-115, 5), L"overflow", renderer::colors::red_color, renderer::fonts::watermark_font);

		sdk::player_t local(sdk::client_info_base + (sdk::local_index() * offsets::player::size));
		auto local_pos = local.get_pos();
		auto local_team = local.team_id();

		for (int i = 0; i < 150; i++) {
			sdk::player_t player(sdk::client_info_base + (i * offsets::player::size));
			if (!player.is_valid() || player.dead()) {
				continue;
			}

			if (player.team_id() == local_team) {
				continue;
			}

			vec2_t screen; vec3_t pos = player.get_pos();
			if (sdk::w2s(pos, screen)) {
				auto dist = sdk::units_to_m(local_pos.distance_to(pos));

				if (dist < 225) {
					wchar_t buf[6];
					swprintf(buf, sizeof(buf), L"[%.1f]m", dist); // not very efficent
					renderer::scene::text(screen, buf, renderer::colors::red_color, renderer::fonts::tahoma_font);					
				}
			}
		}
		renderer::scene::end();

		sdk::ref_def = driver::read<sdk::ref_def_t>(decryption::get_ref_def());
	}
	renderer::shutdown();

	// no recoil
	//while (true) {
	//	if (GetAsyncKeyState(VK_LBUTTON)) {
	//		DWORD64 r12 = client_info + offsets::other::recoil;
	//		DWORD64 rsi = r12 + 0x4;
	//		DWORD edx = driver::read<DWORD64>(r12 + 0xC);
	//		DWORD ecx = edx;
	//		ecx = ecx ^ (DWORD)r12;
	//		DWORD eax = ecx + 2;
	//		eax = eax * ecx;
	//		ecx = edx;
	//		ecx = ecx ^ (DWORD)rsi;
	//		driver::write(r12, eax, sizeof(float));
	//		eax = ecx + 2;
	//		eax = ecx + 2;
	//		eax = eax * ecx;
	//		driver::write(rsi, eax, sizeof(float));
	//	}
	//}

	return 0;
}
