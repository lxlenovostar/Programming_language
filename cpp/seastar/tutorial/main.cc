#include "core/app-template.hh"
#include <iostream>
#include <stdexcept>
extern future<> f();
int main(int argc, char** argv) {
    app_template app;
    try {
        app.run(argc, argv, f);
    } catch(std::runtime_error &e) {
        std::cerr << "Couldn't start application: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
