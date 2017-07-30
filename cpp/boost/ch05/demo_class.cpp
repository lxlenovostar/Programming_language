#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

class demo_class
{
	friend std::ostream& operator<< (std::ostream& os, const demo_class& x)
	{
		os << "demo_class's Name";
		return os;
	}	
};

int main()
{
		std::cout << lexical_cast<std::string>(demo_class()) << std::endl;
}
