#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <typeinfo>

using namespace std;
using namespace boost;

template<typename T>
struct outable
{
	friend std::ostream& operator<<(std::ostream& os, const T& x)
	{
		os << typeid(T).name();
		return os;
	}	
};

class demo_class : outable<demo_class>
{};

int main()
{
	std::cout << lexical_cast<std::string>(demo_class()) << std::endl;
}
