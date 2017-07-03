#include <boost/asio.hpp>
//#include <boost/system/error_code.hpp> 

//g++ -o test -g -Wall asio_timer.cpp -lboost_system

using namespace boost::asio;
using namespace std;

int main()
{
	ip::address addr;
	addr = addr.from_string("127.0.0.1");
	assert(addr.is_v4());
	cout << addr.to_string() << endl;

	ip::tcp::endpoint ep(addr, 6688);
	assert(ep.address() == addr);
	assert(ep.port() == 6688);

	addr = addr.from_string("ab::12:34:56");
	assert(addr.is_v6());
}
