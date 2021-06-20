#include "sdk.h"
#include "..\driver\driver.h"
#include "offsets.h"

namespace sdk {
	HANDLE	  process_id = NULL;
	uintptr_t module_base = NULL;
	uintptr_t peb = NULL;
	HWND      hwnd = NULL;

	uintptr_t client_info = NULL;
	uintptr_t client_info_base = NULL;

	BOOL CALLBACK enum_windows(HWND hwnd, LPARAM param) {
		DWORD process_id;
		GetWindowThreadProcessId(hwnd, &process_id);
		if (process_id == param)
		{
			sdk::hwnd = hwnd;
			return false;
		}
		return true;
	}

	void set_game_hwnd() {
		EnumWindows(enum_windows, (LPARAM)sdk::process_id);
	}

	bool in_game() {
		return driver::read<int>(sdk::module_base + offsets::game_mode) > 1;
	}

	int player_count() {
		return driver::read<uintptr_t>(sdk::module_base + offsets::game_mode);
	}

	int local_index() {
		auto local_index = driver::read<uintptr_t>(sdk::client_info + offsets::local_index);
		return driver::read<int>(local_index + 0x1FC);
	}
		
	bool player_t::is_valid() {
		return driver::read<bool>(address + offsets::player::valid);
	}
		
	bool player_t::dead() {
		auto dead1 = driver::read<bool>(address + offsets::player::dead_1);
		auto dead2 = driver::read<bool>(address + offsets::player::dead_2);
		return dead1 || dead2;
	}

	int player_t::team_id() {
		return driver::read<int>(address + offsets::player::team_id);
	}

	vec3_t player_t::get_pos() {
		auto local_pos = driver::read<uintptr_t>(address + offsets::player::pos_info);
		return driver::read<vec3_t>(local_pos + 0x40);
	}

	vec3_t get_camera_position() {
		auto camera = driver::read<uintptr_t>(sdk::module_base + offsets::camera_base);
		if (!camera)
			return {};

		return driver::read<vec3_t>(camera + offsets::camera_pos);
	}

	bool world_to_screen(vec3_t world_location, vec2_t& out, vec3_t camera_pos, int screen_width, int screen_height, vec2_t fov, vec3_t matricies[3]) {
		auto local = world_location - camera_pos;
		auto trans = vec3_t{
			local.dot(matricies[1]),
			local.dot(matricies[2]),
			local.dot(matricies[0])
		};

		if (trans.z < 0.01f) {
			return false;
		}

		out.x = ((float)screen_width / 2.0) * (1.0 - (trans.x / fov.x / trans.z));
		out.y = ((float)screen_height  / 2.0) * (1.0 - (trans.y / fov.y / trans.z));

		if (out.x < 1 || out.y < 1 || (out.x > sdk::ref_def.width) || (out.y > sdk::ref_def.height)) {
			return false;
		}

		return true;
	}

	bool w2s(vec3_t world_position, vec2_t& screen_position) {
		return world_to_screen(world_position, screen_position, get_camera_position(), ref_def.width, ref_def.height, ref_def.view.tan_half_fov, ref_def.view.axis);
	}

	float units_to_m(float units) {
		return units * 0.0254;
	}

	ref_def_t ref_def;
}

namespace decryption {
	uintptr_t get_client_info() {
		uint64_t RAX = 0, RBX = 0, RCX = 0, RDX = 0, R8 = 0, RDI = 0, RSI = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0, R15 = 0;

		RBX = driver::read<uint64_t>(sdk::module_base + 0x17224118);
		R8 = sdk::peb; // mov r8,gs:[rax]
		RAX = sdk::module_base;
		RBX += RAX;
		RAX = RBX;
		RAX >>= 0x10;
		RBX ^= RAX;
		RAX = RBX;
		RCX = 0x0;
		RAX >>= 0x20;
		RAX ^= RBX;
		RCX = _rotl64(RCX, 0x10);
		RCX ^= driver::read<uint64_t>(sdk::module_base + 0x660D10A);
		RCX = _byteswap_uint64(RCX);
		RBX = driver::read<uint64_t>(RCX + 0x13);
		RBX *= RAX;
		RAX = 0x76D048452DCF6909;
		RBX -= R8;
		R8 = ~R8;
		RBX *= RAX;
		RAX = sdk::module_base + 0x2C8CD073;
		R8 += RAX;
		RBX ^= R8;
		return RBX;
	}

	uintptr_t get_client_info_base() {
		uint64_t RAX = 0, RBX = 0, RCX = 0, RDX = 0, RSP = 0, R8 = 0, RDI = 0, RSI = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0, R15 = 0;

		RAX = driver::read<uint64_t>(sdk::client_info + offsets::client_base);
		RBX = sdk::peb; // mov rbx,gs:[rcx]
		// test rax,rax
		// je 00007FF77524D6C8h
		RCX = RBX;
		RCX <<= 0x21;
		RCX = _byteswap_uint64(RCX);
		RCX &= 0xF;
		// cmp rcx,0Eh
		// ja 00007FF77524D2A4h
		switch (RCX) {
		case 0: {
			R9 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R9;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = RAX;
			RCX >>= 0x18;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x30;
			RAX ^= RCX;
			RCX = sdk::module_base;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x8;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x10;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x20;
			RAX ^= RCX;
			RCX = 0x345963FE4F9F5BC7;
			RAX *= RCX;
			RCX = 0x1BC0D0E9288C6DB3;
			RAX += RCX;
			RAX += RBX;
			RCX = sdk::module_base;
			RAX -= RCX;
			return RAX;
		}
		case 1: {
			R11 = sdk::module_base + 0x6064722A;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = RBX;
			RCX ^= R11;
			RAX -= RCX;
			RCX = RAX;
			RCX >>= 0x11;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x22;
			RAX ^= RCX;
			RAX ^= RBX;
			RCX = sdk::module_base + 0x28AB;
			RCX = ~RCX;
			RCX -= RBX;
			RAX += RCX;
			RCX = 0x16A1C31B3D93A83F;
			RAX *= RCX;
			RCX = 0xD0C234BF8A55764B;
			RAX *= RCX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R10;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = 0xB75E6F62B4DBBCC1;
			RAX *= RCX;
			return RAX;
		}
		case 2: {
			R14 = sdk::module_base + 0x30A5;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = RAX;
			RCX >>= 0x15;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x2A;
			RAX ^= RCX;
			RCX = RBX;
			RCX = ~RCX;
			RCX ^= R14;
			RAX -= RCX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R10;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = 0x5D11A30DE94FFEDE;
			RAX += RCX;
			RCX = RAX;
			RCX >>= 0x1B;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x36;
			RAX ^= RCX;
			RAX ^= RBX;
			RCX = 0x1D2CA89A1A1BE3D9;
			RAX ^= RCX;
			RCX = 0xDD63D27B22050957;
			RAX *= RCX;
			return RAX;
		}
		case 3: {
			R14 = sdk::module_base + 0x7B3CDBC1;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RDX = RBX;
			RDX = ~RDX;
			RCX = R14;
			RCX = ~RCX;
			RDX *= RCX;
			RAX += RDX;
			RCX = RAX;
			RCX >>= 0x26;
			RCX ^= RAX;
			RAX = RCX + RBX;
			RCX = sdk::module_base;
			RAX -= RCX;
			RAX -= 0x7736E4C5;
			RCX = 0xA4C7B3171334DA2E;
			RAX ^= RCX;
			RCX = 0x667B75570F23711D;
			RAX *= RCX;
			RCX = 0x7E05078E8B5B3EDA;
			RAX -= RCX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R10;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			return RAX;
		}
		case 4: {
			R9 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RAX ^= RBX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R9;
			RCX = _byteswap_uint64(RCX);
			RCX = driver::read<uint64_t>(RCX + 0xB);
			RSP = 0x64DE26759A457153;
			RCX *= RSP;
			RAX *= RCX;
			RCX = RAX;
			RCX >>= 0x24;
			RAX ^= RCX;
			RCX = 0x49AF5B2E74070925;
			RAX *= RCX;
			RCX = 0xB5CC279242DD0301;
			RAX *= RCX;
			return RAX;
		}
		case 5: {
			R11 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			R15 = sdk::module_base + 0x6BA9;
			RDX = sdk::module_base + 0x5F9E55C9;
			RDX = ~RDX;
			RDX ^= RBX;
			RCX = RAX;
			RAX = 0xBF5978C960F6BB4B;
			RAX ^= RCX;
			RAX += RDX;
			RDX = sdk::module_base + 0x28877536;
			RCX = RAX;
			RCX >>= 0x18;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x30;
			RAX ^= RCX;
			RCX = RBX;
			RCX = ~RCX;
			RCX *= R15;
			RAX ^= RCX;
			R8 = 0x0;
			R8 = _rotl64(R8, 0x10);
			R8 ^= R11;
			RCX = RBX;
			RCX *= RDX;
			RDX = RAX;
			RDX -= RCX;
			RCX = 0x84229F2B4FE6843B;
			R8 = _byteswap_uint64(R8);
			RAX = driver::read<uint64_t>(R8 + 0xB);
			RAX *= RDX;
			RAX *= RCX;
			RAX ^= RBX;
			return RAX;
		}
		case 6: {
			R15 = sdk::module_base + 0xE397;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = sdk::module_base;
			RAX += RCX;
			RCX = sdk::module_base;
			RAX += RCX;
			RCX = 0x4030351D523D85BB;
			RAX += RCX;
			RCX = RBX;
			RCX ^= R15;
			RAX += RCX;
			RCX = 0x71A01F36E5BF55AF;
			RAX *= RCX;
			RCX = RAX;
			RCX >>= 0x10;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x20;
			RCX ^= RAX;
			RDX = 0x0;
			RDX = _rotl64(RDX, 0x10);
			RDX ^= R10;
			RDX = _byteswap_uint64(RDX);
			RAX = driver::read<uint64_t>(RDX + 0xB);
			RDX = sdk::module_base + 0x31AFF9CE;
			RAX *= RCX;
			RCX = RBX;
			RCX *= RDX;
			RAX -= RCX;
			return RAX;
		}
		case 7: {
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			R15 = sdk::module_base + 0x9CF0;
			RCX = RAX;
			RCX >>= 0x19;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x32;
			RAX ^= RCX;
			RCX = sdk::module_base;
			RCX += 0x16E9;
			RCX += RBX;
			RAX ^= RCX;
			RDX = 0x0;
			RDX = _rotl64(RDX, 0x10);
			RDX ^= R10;
			RCX = RBX;
			RDX = _byteswap_uint64(RDX);
			RCX ^= R15;
			RDX = driver::read<uint64_t>(RDX + 0xB);
			RAX *= RDX;
			RAX -= RCX;
			RCX = RAX;
			RCX >>= 0xA;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x14;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x28;
			RAX ^= RCX;
			RCX = 0x201300BD919020EB;
			RAX *= RCX;
			RCX = 0x136871F8B2311042;
			RAX += RCX;
			RCX = 0xE0229051A9F3C38B;
			RAX ^= RCX;
			return RAX;
		}
		case 8: {
			R15 = sdk::module_base + 0x6C04;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R10;
			RCX = _byteswap_uint64(RCX);
			RDX = driver::read<uint64_t>(RCX + 0xB);
			RCX = 0x866F75E98D0D53B1;
			RDX *= RAX;
			RAX = RBX;
			RAX *= R15;
			RDX += RAX;
			RDX ^= RBX;
			RAX = RDX;
			RAX >>= 0x20;
			RAX ^= RDX;
			RAX ^= RCX;
			RCX = 0x9E0D951F0C28F90B;
			RAX *= RCX;
			RCX = 0x78503CB374B04FAD;
			RAX *= RCX;
			RCX = RAX;
			RCX >>= 0x2;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x4;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x8;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x10;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x20;
			RAX ^= RCX;
			return RAX;
		}
		case 9: {
			R14 = sdk::module_base + 0x6CFB74E0;
			R11 = sdk::module_base + 0x7F309832;
			R9 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R9;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RAX ^= RBX;
			RAX ^= R14;
			RCX = RAX;
			RCX >>= 0x17;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x2E;
			RAX ^= RCX;
			RCX = 0xD7356E290A5B1FBA;
			RAX += RCX;
			RCX = sdk::module_base;
			RAX ^= RCX;
			RCX = 0xD80D8A31210F08D3;
			RAX *= RCX;
			RCX = R11;
			RCX = ~RCX;
			RCX ^= RBX;
			RAX -= RCX;
			RCX = RAX;
			RCX >>= 0x9;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x12;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x24;
			RAX ^= RCX;
			return RAX;
		}
		case 10: {
			R9 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			R11 = sdk::module_base + 0x6AD2A7C4;
			RAX -= RBX;
			RAX ^= RBX;
			RCX = 0x29222BE3E0E2FFB;
			RAX ^= R11;
			R11 = sdk::module_base;
			RAX *= RCX;
			RCX = 0x5BB04B85CD9365D;
			RAX -= RBX;
			RAX += RCX;
			RAX += R11;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R9;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = 0x5FC588EC700475F3;
			RAX *= RCX;
			RCX = RAX;
			RCX >>= 0xC;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x18;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x30;
			RAX ^= RCX;
			return RAX;
		}
		case 11: {
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			R14 = sdk::module_base + 0xCF97;
			RDX = R14;
			RDX = ~RDX;
			RDX++;
			RDX += RBX;
			RAX ^= RDX;
			RCX = sdk::module_base + 0xCA22;
			RAX += RBX;
			RDX = sdk::module_base;
			RAX += RCX;
			RCX = RBX;
			RCX = ~RCX;
			RCX -= RDX;
			RCX -= 0x1236;
			RAX ^= RCX;
			RCX = 0x48502E6384BA9941;
			RAX *= RCX;
			RCX = 0x5EB925E16D423E1E;
			RAX -= RCX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R10;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = 0xE5AB625D3BB65BBF;
			RAX *= RCX;
			RCX = RAX;
			RCX >>= 0x1F;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x3E;
			RAX ^= RCX;
			return RAX;
		}
		case 12: {
			R15 = sdk::module_base + 0xEE34;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = RBX + 1;
			RCX *= R15;
			RAX += RCX;
			RAX ^= RBX;
			RCX = 0xBF0F6EC504339C71;
			RAX *= RCX;
			RCX = 0x62753D45ABF968CD;
			RAX -= RCX;
			RCX = 0x28C82E52D21EB6AB;
			RAX -= RCX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R10;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = RAX;
			RCX >>= 0xB;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x16;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x2C;
			RAX ^= RCX;
			RCX = sdk::module_base;
			RAX ^= RCX;
			return RAX;
		}
		case 13: {
			R10 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R10;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = RAX;
			RCX >>= 0x2;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x4;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x8;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x10;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x20;
			RAX ^= RCX;
			RDX = RAX;
			RDX >>= 0x22;
			RDX ^= RAX;
			RCX = 0xAB96BD5255F50EEF;
			RAX = sdk::module_base + 0x4795B778;
			RAX = ~RAX;
			RAX ^= RBX;
			RAX += RDX;
			RAX *= RCX;
			RAX -= RBX;
			RCX = 0x697DECF064AB09C3;
			RAX *= RCX;
			RCX = RBX;
			RSP = sdk::module_base + 0xE842;
			RCX *= RSP;
			RAX += RCX;
			return RAX;
		}
		case 14: {
			R9 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = RAX;
			RCX >>= 0xB;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x16;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x2C;
			RAX ^= RCX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R9;
			RCX = _byteswap_uint64(RCX);
			RCX = driver::read<uint64_t>(RCX + 0xB);
			RSP = 0xF2B84228009F892B;
			RCX *= RSP;
			RAX *= RCX;
			R10 = 0x21D0F0E2660F5094;
			RCX = RBX;
			RCX = ~RCX;
			RSP = sdk::module_base + 0x2E00;
			RCX *= RSP;
			RCX += R10;
			RAX += RCX;
			RCX = RAX;
			RCX >>= 0x10;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x20;
			RAX ^= RCX;
			RCX = 0x1E450D45A88B3DC9;
			RAX *= RCX;
			RCX = RAX;
			RCX >>= 0x17;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x2E;
			RAX ^= RCX;
			return RAX;
		}
		case 15: {
			R9 = driver::read<uint64_t>(sdk::module_base + 0x660D140);
			RCX = sdk::module_base;
			RAX ^= RCX;
			RCX = 0x104FF8B4C43406AD;
			RAX += RCX;
			RCX = 0x16DB4431461A3E29;
			RAX *= RCX;
			RCX = 0x0;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= R9;
			RCX = _byteswap_uint64(RCX);
			RAX *= driver::read<uint64_t>(RCX + 0xB);
			RCX = RAX;
			RCX >>= 0x13;
			RAX ^= RCX;
			RCX = RAX;
			RCX >>= 0x26;
			RAX ^= RCX;
			RCX = sdk::module_base;
			RAX -= RCX;
			RAX += 0xFFFFFFFFFFFF9A85;
			RAX += RBX;
			RCX = 0x11B2D7215841BEB4;
			RSP = sdk::module_base;
			RCX -= RSP;
			RAX += RCX;
			return RAX;
		}
		default:
			return 0;
		}
	}

	uint64_t get_bone() {
		uint64_t RAX = sdk::module_base, RBX = sdk::module_base, RCX = sdk::module_base, RDX = sdk::module_base, R8 = sdk::module_base, RDI = sdk::module_base, R9 = sdk::module_base, R10 = sdk::module_base, R11 = sdk::module_base, R12 = sdk::module_base, R13 = sdk::module_base, R14 = sdk::module_base, R15 = sdk::module_base, RSI = sdk::module_base, RSP = sdk::module_base, RBP = sdk::module_base;
		RDX = driver::read<uint64_t>(sdk::module_base + 0x15B582C8);
		
		if (!RDX)
			return 0;
		R8 = sdk::peb;
		R8 = (~R8);
		// test rdx,rdx
		// je near ptr 00000000024FFDFEh
		RAX = R8;
		RAX = _rotl64(RAX, 0x28);
		RAX &= 0xF;
		switch (RAX)
		{
		case 0:
		{
			R14 = sdk::module_base + 0x4D63FF68;
			R11 = sdk::module_base + 0xE7E;
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			RAX = 0x38330DCD6D3EE86B;
			RDX *= RAX;
			RAX = 0x62FA53C3F4793FE6;
			RDX ^= RAX;
			RAX = R8;
			RAX = (~RAX);
			RDX += RAX;
			RAX = sdk::module_base + 0x572A;
			RDX += RAX;
			RAX = RDX;
			RAX >>= 0x1A;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x34;
			RDX ^= RAX;
			RCX = R14;
			RCX = (~RCX);
			RCX += R8;
			RAX = 0xBA6195B9FE5C98BF;
			RAX += RDX;
			RDX = RCX;
			RDX ^= RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= RBX;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			RDX -= R8;
			return RDX;
		}
		case 1:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			
			R11 = sdk::module_base + 0xE7E;
			R12 = sdk::module_base + 0x3FC;
			RCX = R8 + R12;
			RAX = RDX + R8;
			RDX = RCX;
			RDX ^= RAX;
			RAX = 0xB0D1385078A56D19;
			RDX *= RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= RBX;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RAX = driver::read<uint64_t>(RAX + 0x15);
			RDX *= RAX;
			RAX = sdk::module_base;
			RDX -= RAX;
			RAX = RDX;
			RAX >>= 0x1D;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x3A;
			RDX ^= RAX;
			RAX = 0xD1151DF1B15559FB;
			RDX *= RAX;
			RAX = 0x14B61D0484FBBE03;
			RDX *= RAX;
			return RDX;
		}
		case 2:
		{
			R11 = sdk::module_base + 0xE7E;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			RAX = 0xCC268FDEC36DA697;
			RDX *= RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= R10;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			RAX = sdk::module_base + 0x40F5FF82;
			RAX = (~RAX);
			RAX -= R8;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x19;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x32;
			RDX ^= RAX;
			RAX = 0x70067EEE063A673C;
			RDX += RAX;
			RDX -= R8;
			RAX = sdk::module_base;
			RDX ^= RAX;
			return RDX;
		}
		case 3:
		{
			R11 = sdk::module_base + 0xE7E;
			R9 = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			RAX = RDX;
			RAX >>= 0x12;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x24;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x1F;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x3E;
			RDX ^= RAX;
			RAX = 0x57A81BEC6516033D;
			RDX += RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= R9;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			RAX = RDX;
			RAX >>= 0x7;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0xE;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x1C;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x38;
			RDX ^= RAX;
			RAX = 0x96699980B562505;
			RDX *= RAX;
			RAX = sdk::module_base;
			RDX ^= RAX;
			return RDX;
		}
		case 4:
		{
			R11 = sdk::module_base + 0xE7E;
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			RAX = 0x1B512279D332955D;
			RDX -= RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= RBX;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			RAX = RDX;
			RAX >>= 0x22;
			RDX ^= RAX;
			RDX ^= R8;
			RAX = 0x3EE6F0C8BA258B0A;
			RDX += RAX;
			RAX = sdk::module_base + 0x2415;
			RAX -= R8;
			RDX += RAX;
			RAX = 0x8A1CDFF81DB6DFF;
			RDX *= RAX;
			RDX += R8;
			return RDX;
		}
		case 5:
		{
			R10 = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= R10;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			RAX = sdk::module_base;
			RDX -= RAX;
			RAX = 0x547FD4FF3C645CA9;
			RDX *= RAX;
			RAX = RDX;
			RAX >>= 0xF;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x1E;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x3C;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0xC;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x18;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x30;
			RDX ^= RAX;
			RAX = 0x781D9F1F2D40889D;
			RDX += RAX;
			RAX = sdk::module_base;
			RDX -= RAX;
			RAX = 0x954A92102762092A;
			RDX ^= RAX;
			return RDX;
		}
		case 6:
		{
			R11 = sdk::module_base + 0xE7E;
			R14 = sdk::module_base + 0x4F53;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			RAX = 0x7C9D860BE61BBC43;
			RDX *= RAX;
			RAX = R14;
			RAX *= R8;
			RDX -= RAX;
			RDX ^= R8;
			RDX += R8;
			RAX = RDX;
			RAX >>= 0x1F;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x3E;
			RDX ^= RAX;
			RAX = 0x7034B0F1930C5EDC;
			RDX -= RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= R10;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RAX = driver::read<uint64_t>(RAX + 0x15);
			RDX *= RAX;
			RAX = 0x7B2C304B4173C6F7;
			RDX -= RAX;
			return RDX;
		}
		case 7:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			R12 = sdk::module_base + 0xB0FF;
			RDX -= R8;
			RAX = RDX;
			RAX >>= 0x28;
			RDX ^= RAX;
			RAX = 0xF27E20A569493D17;
			RDX *= RAX;
			RAX = 0x4A436F8C9A9684F2;
			RDX += RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= RBX;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RAX = driver::read<uint64_t>(RAX + 0x15);
			RSP = 0x746D10714AE9CA49;
			RAX *= RSP;
			RDX *= RAX;
			RCX = R12;
			RCX = (~RCX);
			RAX = R8;
			RAX = (~RAX);
			RCX *= RAX;
			RDX += RCX;
			RAX = RDX;
			RAX >>= 0x1D;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x3A;
			RDX ^= RAX;
			return RDX;
		}
		case 8:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			R13 = sdk::module_base + 0x1987;
			R12 = sdk::module_base + 0x59DAB869;
			RAX = sdk::module_base;
			RDX += RAX;
			RCX = sdk::module_base + 0xC5;
			RAX = RDX;
			RCX *= R8;
			RDX = 0xCC9633BEB940B34E;
			RDX ^= RAX;
			RDX += RCX;
			RAX = RDX;
			RAX >>= 0x1B;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x36;
			RDX ^= RAX;
			RAX = 0xAA9085BB5AF94951;
			RDX *= RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= RBX;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			RAX = R8;
			RAX = (~RAX);
			RAX ^= R12;
			RDX -= RAX;
			RAX = R8 + R13;
			RDX += RAX;
			return RDX;
		}
		case 9:
		{
			R11 = sdk::module_base + 0xE7E;
			R10 = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			RAX = RDX;
			RAX >>= 0x28;
			RDX ^= RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= R10;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RAX = driver::read<uint64_t>(RAX + 0x15);
			RDX *= RAX;
			RAX = sdk::module_base;
			RDX += RAX;
			RAX = sdk::module_base;
			RDX ^= RAX;
			RAX = 0x2EB32E09A3A9483F;
			RDX ^= RAX;
			RAX = sdk::module_base;
			RDX -= R8;
			RDX -= RAX;
			RDX -= 0x4CF2;
			RAX = sdk::module_base;
			RDX += RAX;
			RAX = 0x5B85E9BB26723E73;
			RDX *= RAX;
			return RDX;
		}
		case 10:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			RAX = R8;
			RAX -= sdk::module_base;
			RAX += 0xFFFFFFFFCDBB51AB;
			RDX += RAX;
			RAX = RDX;
			RAX >>= 0x17;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x2E;
			RDX ^= RAX;
			RAX = 0xC12A2DA65C4961CD;
			RDX *= RAX;
			RAX = sdk::module_base;
			RDX += RAX;
			RAX = 0xF9373B84DED5636B;
			RDX ^= RAX;
			RAX = 0x91CDED26ACFB0DBB;
			RDX ^= RAX;
			RAX = RDX;
			// mov rcx,[rbp+598h]
			RDX = sdk::module_base;
			RCX -= R11;
			RAX ^= RDX;
			RCX = 0; // Special case
			RCX = _rotl64(RCX, 0x10);
			RCX ^= RBX;
			RCX = (~RCX);
			if (!((void*)(RCX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX = driver::read<uint64_t>(RCX + 0x15);
			RDX *= RAX;
			return RDX;
		}
		case 11:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			R15 = sdk::module_base + 0x5A01ECB2;
			RAX = RDX;
			RAX >>= 0x11;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x22;
			RDX ^= RAX;
			RAX = 0xDE0B65ACC065BF3F;
			RDX *= RAX;
			RAX = sdk::module_base;
			RDX ^= RAX;
			RAX = 0x1641BCAE40423EA9;
			RDX *= RAX;
			RCX = sdk::module_base + 0x1;
			RAX = sdk::module_base + 0x1AF73A5E;
			RAX = (~RAX);
			RAX += R8;
			RCX += RAX;
			RDX += RCX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= RBX;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			RAX = R15;
			RAX *= R8;
			RDX -= RAX;
			return RDX;
		}
		case 12:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			R12 = sdk::module_base + 0xE486;
			RAX = RDX;
			RAX >>= 0x10;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x20;
			RDX ^= RAX;
			RAX = 0xF9625ECE5BB27C2F;
			RDX *= RAX;
			RAX = RDX;
			RAX >>= 0x25;
			RDX ^= RAX;
			RAX = sdk::module_base;
			RDX -= RAX;
			RAX = sdk::module_base + 0xE0F2;
			RAX = (~RAX);
			RAX *= R8;
			RDX ^= RAX;
			// mov rcx,[rbp+598h]
			RCX -= R11;
			RCX = 0; // Special case
			RAX = RDX;
			RCX = _rotl64(RCX, 0x10);
			RCX ^= RBX;
			RDX = 0xD0C187FCC55A5035;
			RAX ^= RDX;
			RCX = (~RCX);
			if (!((void*)(RCX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX = driver::read<uint64_t>(RCX + 0x15);
			RDX *= RAX;
			RAX = R12;
			RAX ^= R8;
			RDX -= RAX;
			return RDX;
		}
		case 13:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			R12 = sdk::module_base + 0xEEA3;
			RAX = 0x5C08B7D7B4223813;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x1C;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x38;
			RDX ^= RAX;
			RAX = R12;
			RAX ^= R8;
			RDX += RAX;
			RAX = 0x1D4CED329660DAAB;
			RDX *= RAX;
			// mov rcx,[rbp+598h]
			RCX -= R11;
			RCX = 0; // Special case
			RCX = _rotl64(RCX, 0x10);
			RCX ^= RBX;
			RAX = sdk::module_base;
			RDX -= RAX;
			RCX = (~RCX);
			RAX = R8 + 0xFFFFFFFFD665307A;
			RAX += RDX;
			if (!((void*)(RCX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX = driver::read<uint64_t>(RCX + 0x15);
			RDX *= RAX;
			RAX = RDX;
			RAX >>= 0xC;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x18;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x30;
			RDX ^= RAX;
			RAX = sdk::module_base;
			RDX += RAX;
			return RDX;
		}
		case 14:
		{
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			R11 = sdk::module_base + 0xE7E;
			R15 = sdk::module_base + 0x1E21A51C;
			R13 = sdk::module_base + 0x225E76E1;
			R12 = sdk::module_base + 0x1CCE7CED;
			RAX = R15;
			RAX *= R8;
			RDX -= RAX;
			RDX += R8;
			RCX = R8;
			RCX = (~RCX);
			RDX += R13;
			RDX += RCX;
			RAX = RDX;
			RAX >>= 0xF;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x1E;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x3C;
			RDX ^= RAX;
			RAX = 0xC1EAFB913E323975;
			RDX *= RAX;
			RAX = sdk::module_base;
			RDX -= RAX;
			RAX = R12;
			RAX *= R8;
			RDX += RAX;
			// mov rax,[rbp+598h]
			RAX -= R11;
			RAX = 0; // Special case
			RAX = _rotl64(RAX, 0x10);
			RAX ^= RBX;
			RAX = (~RAX);
			if (!((void*)(RAX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX *= driver::read<uint64_t>(RAX + 0x15);
			return RDX;
		}
		case 15:
		{
			R11 = sdk::module_base + 0xE7E;
			R15 = sdk::module_base + 0x3EF00DB9;
			R12 = sdk::module_base + 0x49C6D582;
			R13 = sdk::module_base + 0x10E06777;
			RBX = driver::read<uint64_t>(sdk::module_base + 0x6CC41F9);
			RAX = 0xF02A39098305851B;
			RDX *= RAX;
			RAX = 0x42A2DDF2A13458E5;
			RDX *= RAX;
			RAX = R12;
			RAX ^= R8;
			RAX += R8;
			RDX += RAX;
			RAX = RDX;
			RAX >>= 0x14;
			RDX ^= RAX;
			RAX = RDX;
			RAX >>= 0x28;
			RDX ^= RAX;
			RAX = R8 + R15;
			RDX ^= RAX;
			// mov rcx,[rbp+598h]
			RCX -= R11;
			RCX = 0; // Special case
			RCX = _rotl64(RCX, 0x10);
			RAX = R8;
			RAX = (~RAX);
			RCX ^= RBX;
			RAX *= R13;
			RCX = (~RCX);
			RAX ^= RDX;
			if (!((void*)(RCX + 0x15))) return 0xFFFFFFFFFFFFFFFF; RDX = driver::read<uint64_t>(RCX + 0x15);
			RDX *= RAX;
			return RDX;
		}
		default:
			return 0;
		}
	}

	uint32_t get_bone_index(uint32_t index) {
		uint64_t RAX = 0, RBX = 0, RCX = 0, RDX = 0, R8 = 0, RDI = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0, RSI = 0, RSP = 0, RBP = 0;
		RBX = index;
		RCX = RBX * 0x13C8;
		RAX = 0x76BCF2ED12008595;
		RBX = sdk::module_base;
		RAX = _umul128(RAX, RCX, &RDX);
		R10 = 0xE09B4B5F1CC2E2BF;
		RDX >>= 0xC;
		RAX = RDX * 0x227F;
		RCX -= RAX;
		RAX = 0xCED712414B44733D;
		R8 = RCX * 0x227F;
		RAX = _umul128(RAX, R8, &RDX);
		RDX >>= 0xD;
		RAX = RDX * 0x279B;
		R8 -= RAX;
		RAX = 0x8ACAD5AD8637C643;
		RAX = _umul128(RAX, R8, &RDX);
		RCX = R8;
		R8 &= 0x1;
		RDX >>= 0xC;
		RAX = RDX * 0x1D83;
		RCX -= RAX;
		RAX = R8 + RCX * 0x2;
		RAX = driver::read<uint16_t>(RBX + RAX * 0x2 + 0x6CCE950);
		R8 = RAX * 0x13C8;
		RAX = R10;
		RAX = _umul128(RAX, R8, &RDX);
		RAX = R10;
		RDX >>= 0xD;
		RCX = RDX * 0x2479;
		R8 -= RCX;
		R9 = R8 * 0x48AA;
		RAX = _umul128(RAX, R9, &RDX);
		RDX >>= 0xD;
		RAX = RDX * 0x2479;
		R9 -= RAX;
		RAX = 0x204081020408103;
		RAX = _umul128(RAX, R9, &RDX);
		RAX = R9;
		RAX -= RDX;
		RAX >>= 0x1;
		RAX += RDX;
		RAX >>= 0x7;
		RCX = RAX * 0xFE;
		RAX = 0xCD85689039B0AD13;
		RAX = _umul128(RAX, R9, &RDX);
		RAX = R9;
		RAX -= RDX;
		RAX >>= 0x1;
		RAX += RDX;
		RAX >>= 0x6;
		RCX += RAX;
		RAX = RCX * 0x8E;
		RCX = R9 + R9 * 0x8;
		RCX <<= 0x4;
		RCX -= RAX;
		RSI = driver::read<uint16_t>(RCX + RBX + 0x6CD5F60);
		
		return RSI;
	}

	struct ref_def_key {
		int ref0;
		int ref1;
		int ref2;
	};

	uintptr_t get_ref_def() {
		ref_def_key crypt = driver::read<ref_def_key>(sdk::module_base + offsets::ref_def_ptr);
		uint64_t baseAddr = sdk::module_base;

		DWORD lower = crypt.ref0 ^ (crypt.ref2 ^ (uint64_t)(baseAddr + offsets::ref_def_ptr)) * ((crypt.ref2 ^ (uint64_t)(baseAddr + offsets::ref_def_ptr)) + 2);
		DWORD upper = crypt.ref1 ^ (crypt.ref2 ^ (uint64_t)(baseAddr + offsets::ref_def_ptr + 0x4)) * ((crypt.ref2 ^ (uint64_t)(baseAddr + offsets::ref_def_ptr + 0x4)) + 2);

		return (uint64_t)upper << 32 | lower; 
	}
}
