#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace boost;
using namespace std;

// g++ -g -Wall lexcical_cast_5_1.cpp 


template<typename T>
bool num_valid(const char *str)
{
	try 
	{
		lexical_cast<T>(str);
		return true;
	}
	catch(bad_lexical_cast&)
	{
		return false;
	}
}

int main()
{
	/*try 
	{
		int x = lexical_cast<int>("100");
		long y = lexical_cast<long>("2000");
		float pai = lexical_cast<float>("3.14159e5");
		double e = lexical_cast<double>("2.71828L");

		cout << x << y << pai << e << endl;

		string str = lexical_cast<string>(456);
		cout << str << endl;
		cout << lexical_cast<string>(0.618) << endl; 
		cout << lexical_cast<string>(0x10) << endl;

		cout << lexical_cast<bool>("1") << endl;
	}
	catch (bad_lexical_cast& e)
	{
		cout << "error:" << e.what() << endl;
	}
	*/

	assert(num_valid<double>("3.14"));
	assert(!num_valid<int>("3.14"));
	assert(num_valid<int>("65535"));
}
