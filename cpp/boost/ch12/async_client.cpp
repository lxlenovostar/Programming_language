#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp> 
#include <boost/function.hpp> 
#include <boost/bind.hpp> 
#include <boost/shared_ptr.hpp>
#include <string>

//g++ -o test -g -Wall asio_timer.cpp -lboost_system

using namespace boost::asio;
using namespace std;

class client 
{
	private:
		io_service& ios;
		ip::tcp::endpoint ep;
		typedef boost::shared_ptr<ip::tcp::socket> sock_pt;
	
	public:
		client(io_service& io): ios(io),
			ep(ip::address::from_string("127.0.0.1"), 6688)
		{
			//启动异步连接
			start();
		}

		void start() 
		{
			//创建socket对象
			sock_pt sock(new ip::tcp::socket(ios));
			sock->async_connect(ep, boost::bind(&client::conn_handler, this, placeholders::error, sock));
		}

		void conn_handler(const boost::system::error_code& ec, sock_pt sock)
		{
			if (ec)
			{
				return;
			}

			cout << "receive from" << sock->remote_endpoint().address();
			boost::shared_ptr< vector<char> > str(new vector<char>(100, 0));

			sock->async_read_some(buffer(*str), bind(&client::read_handler, this, placeholders::error, str));

			//再次启动异步连接
			start();
		}

		void read_handler(const boost::system::error_code& ec, boost::shared_ptr< vector<char> > str)
		{
			if (ec)
			{
				return;
			}

			//输出接收到的数据
			cout << &(*str)[0] << endl;
		}
};

int main()
{
	try
	{
		cout << "client start." << endl;
		io_service ios;
		client cl(ios);
		ios.run();
	}
	catch(std::exception& e)
	{
		cout << e.what() << endl;
	}
}
