#pragma once
#include <cstdint>
#include <memory>
#include <string_view>
#include <windows.h>
#include <TlHelp32.h>
#include <mutex>
#include <iostream>
#include "../game/sdk.h"

typedef struct _COPY_MEMORY {
	void*		buffer;
	ULONG64		address;
	ULONG		size;
	HANDLE		pid;
	bool		get_pid;
	bool		base;
	bool		peb;
	bool		read;
	bool		write;
	const char* module_name;
	const char* process_name;
}COPY_MEMORY;

namespace driver {
	static std::once_flag flag;

	template<typename ... A>
	uint64_t call_hook(const A ... arguments)
	{
		std::call_once(flag, [] { LoadLibrary("user32.dll"); });
		void* control_function = GetProcAddress(LoadLibrary("win32u.dll"), "NtOpenCompositionSurfaceSectionInfo");
		const auto control = static_cast<uint64_t(__stdcall*)(A...)>(control_function);
		return control(arguments ...);
	}

	static HANDLE get_process_id(const char* process_name) {
		COPY_MEMORY m{};
		m.get_pid = true;
		m.process_name = process_name;
		call_hook(&m);

		return m.pid;
	}

	static uintptr_t get_module_base_address(const char* module_name) {
		COPY_MEMORY m{};
		m.base = true;
		m.pid = sdk::process_id;
		m.module_name = module_name;
		call_hook(&m);

		return (uintptr_t)m.buffer;
	}

	static uintptr_t get_peb() {
		COPY_MEMORY m{};
		m.peb = true;
		m.pid = sdk::process_id;
		call_hook(&m);

		return (uintptr_t)m.buffer;
	}

	template <typename type>
	type read(ULONG64 address) {
		type buffer{};

		COPY_MEMORY m{};
		m.read = true;
		m.pid = sdk::process_id;
		m.address = address;
		m.buffer = &buffer;
		m.size = sizeof(type);

		call_hook(&m);
		return buffer;
	}

	template <typename type>
	void write(ULONG64 address, type value, ULONG size = sizeof(value))
	{
		COPY_MEMORY m{};
		m.write = true;
		m.pid = sdk::process_id;
		m.address = address;
		m.buffer = &value;
		m.size = size;

		call_hook(&m);
	}
}