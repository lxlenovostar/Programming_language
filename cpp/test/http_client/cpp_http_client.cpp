#include <iostream>
#include <string>
#include "restclient-cpp/restclient.h"

using namespace std;

// g++ -g -Wall cpp_http_client.cpp -o cpp_client  -I/opt/include -L/opt/lib -Wl,-Bstatic  -lrestclient-cpp -Wl,-Bdynamic -lcurl

// g++ -g -Wall cpp_http_client.cpp -o cpp_client  -I/opt/include -L/opt/lib -lrestclient-cpp

int main()
{
	RestClient::Response r = RestClient::get("http://www.baidu.com");

	std::cout << "code:" << r.code << std::endl;
	
	return 0;
}


