#include "core/sleep.hh"
#include <iostream>

future<int> slow() {
	using namespace std::chrono_literals;
	return sleep(100ms).then([] { return 3; });
}

future<> f() {
	return slow().then([] (int val) {
		std::cout << "Got " << val << "\n";
	});
}
