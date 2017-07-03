#include <boost/asio.hpp>
#include <boost/system/error_code.hpp> 

//g++ -o test -g -Wall asio_timer.cpp -lboost_system

using namespace boost::asio;
using namespace std;

int main()
{
	try 
	{
		cout << "server start." << endl;
		io_service ios;

		//创建acceptor对象，接受6688端口
		ip::tcp::acceptor acceptor(ios, ip::tcp::endpoint(ip::tcp::v4(), 6688));
		cout << acceptor.local_endpoint().address() << endl;

		while(true)
		{
			//一个socket对象
			ip::tcp::socket sock(ios);
			//阻塞等待socket连接
			acceptor.accept(sock);

			cout << "client:";
			cout << sock.remote_endpoint().address() << endl;

			//通常我们不能直接把数组、vector等容器作为asio的读写参数，必须使用buffer()函数包装.
			sock.write_some(buffer("hello asio"));
		}
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
}
