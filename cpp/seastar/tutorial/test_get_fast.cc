#include "core/future.hh"
#include <iostream>
future<int> fast() {
	return make_ready_future<int>(3);
}

future<> f() {
	return fast().then([] (int val) {
		std::cout << "Got " << val << "\n";
	});
}
