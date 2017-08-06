// skipws flag example
#include <iostream>     // std::cout, std::skipws, std::noskipws
#include <sstream>      // std::istringstream

int main () 
{
	char a, b, c;

	std::cout << std::unitbuf;
    std::istringstream iss("  123");
    iss >> std::skipws >> a >> b >> c;
    std::cout << a << b << c << '\n';

	std::cout << std::nounitbuf;

    iss.seekg(0);
    iss >> std::noskipws >> a >> b >> c;
    std::cout << a << b << c << '\n';
    return 0;

}
