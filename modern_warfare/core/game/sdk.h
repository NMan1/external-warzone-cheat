#pragma once
#include <cstdint>
#include <windows.h>
#include "..\utils\vectors.h"

namespace sdk {
	extern HANDLE	 process_id;
	extern uintptr_t module_base;
	extern uintptr_t peb;
	extern HWND      hwnd;

	extern uintptr_t client_info;
	extern uintptr_t client_info_base;

	struct ref_def_view {
		vec2_t tan_half_fov;
		char pad[0xC];
		vec3_t axis[3];
	};

	struct ref_def_t {
		int x;
		int y;
		int width;
		int height;
		ref_def_view view;
	};

	extern struct ref_def_t ref_def;

	void set_game_hwnd();

	bool in_game();

	int player_count();

	int local_index();

	class player_t {
	public:
		player_t(uintptr_t address) {
			this->address = address;
		}

		uintptr_t address{};

		bool is_valid();

		bool dead();

		int team_id();

		vec3_t get_pos();
	};

	bool w2s(vec3_t world_position, vec2_t& screen_position);

	float units_to_m(float units);
}

namespace decryption {
	uintptr_t get_client_info();

	uintptr_t get_client_info_base();

	uint64_t get_bone();

	uint32_t get_bone_index(uint32_t index);

	uintptr_t get_ref_def();
}
