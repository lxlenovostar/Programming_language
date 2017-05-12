#include "core/sleep.hh"
#include <iostream>
future<> f() {
    std::cout << "Sleeping... " << std::flush;
    using namespace std::chrono_literals;
    sleep(200ms).then([] { std::cout << "200ms " << std::flush; });
    sleep(100ms).then([] { std::cout << "100ms " << std::flush; });
    return sleep(1s).then([] { std::cout << "Done.\n"; });
}
