#pragma once

/*
UPDATED: 2022-05-12
VERSION: 1.57.3.11521872
OFFSETS SOURCE: Unknowncheats Community (https://www.unknowncheats.me/forum/call-of-duty-modern-warfare/355326-call-duty-modern-warfare-213.html)
Thanks to ilahigod31, Biellz, carrooo1, SerialGod, KANKOSHEV
*/

namespace offsets
{
	constexpr auto camera_base = 0x1B42CCB0;
	constexpr auto camera_pos = 0x1E8;
	constexpr auto game_mode = 0x1D5F01F8;
	constexpr auto local_index = 0x9AA98;
	constexpr auto name_array = 0x1DAC7E40;
	constexpr auto name_array_padding = 0x4C70;
	constexpr auto ref_def_ptr = 0x1DABB9D0;
	constexpr auto score_base = 0x0;
	constexpr auto weapon_definition = 0x1B4F2140;
	constexpr auto client_info = 0x1720EB88;
	constexpr auto client_base = 0x9DBE8;

	namespace bones
	{
		constexpr auto bone_base = 0x4E06C;
		constexpr auto distribute = 0x201DD368;
		constexpr auto size = 0x60C8;
		constexpr auto visible = 0x609CA90;
	}

	namespace directx
	{
		constexpr auto command_queue = 0x1F5910D8;
		constexpr auto swap_chain = 0x0;
	}

	namespace other
	{
		constexpr auto recoil = 0x1B52C;
	}

	namespace player
	{
		constexpr auto dead_1 = 0x594;
		constexpr auto dead_2 = 0x5070;
		constexpr auto pos_info = 0x5068;
		constexpr auto size = 0x60C8;
		constexpr auto stance = 0x56F4;
		constexpr auto team_id = 0x60A4;
		constexpr auto valid = 0x468;
		constexpr auto weapon_index = 0x4FBA;
	}
}
