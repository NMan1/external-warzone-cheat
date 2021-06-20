#include "renderer.h"
#include <iostream>


namespace renderer {
	bool init( ) {
		if (!internal::init()) {
			std::cout << "[-] renderer internal init failed" << std::endl;
			return false;
		}
		
		if (!colors::init()) {
			std::cout << "[-] renderer colors init failed" << std::endl;
			return false;
		}
		
		if (!fonts::init()) {
			std::cout << "[-] renderer fonts init failed" << std::endl;
			return false;
		}

		return true;
	}

	void shutdown() {
		scene::shutdown();
		internal::shutdown();
		colors::shutdown();
		fonts::shutdown();
	}
}
