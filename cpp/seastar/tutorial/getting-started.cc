#include "core/app-template.hh"
#include "core/reactor.hh"
#include <iostream>

int main(int argc, char** argv) {
	app_template app;
	try {
		app.run(argc, argv, [] {
			std::cout << "Hello world\n";
			std::cout << smp::count << "\n";
			return make_ready_future<>();
		}); 
	/*
     * Note that catching the exceptions this way does not catch exceptions 
     * thrown in the application's actual asynchronous code. 
     */
	} catch(std::runtime_error &e) {
		std::cerr << "Couldn't start application: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
