#include "core/sleep.hh"
#include <iostream>
future<int> incr(int i) {
	using namespace std::chrono_literals;
	//return sleep(10ms).then([i] { return i + 1; });
	/*
	 * this would have meant that the continuation would contain the address of i, 
	 * not its value. But i is a stack variable, and the incr() function returns 
	 * immediately, so when the continuation eventually gets to run, long after incr() 
	 * returns, this address will contain unrelated content.
	 * */
	return sleep(10ms).then([&i] { return i + 1; }); // Oops, the "&" here is wrong.
}

future<> f() {
	return incr(3).then([] (int val) {
		std::cout << "Got " << val << "\n";
	});
}
