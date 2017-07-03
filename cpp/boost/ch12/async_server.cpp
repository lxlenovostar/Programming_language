#include <boost/asio.hpp>
#include <boost/system/error_code.hpp> 
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp> 


//g++ -o test -g -Wall asio_timer.cpp -lboost_system

using namespace boost::asio;
using namespace std;

class server 
{
	private:
		//io_service, acceptor是TCP通信必备的要素
		io_service &ios;
		ip::tcp::acceptor acceptor;
		//指向socket对象，用来在回调函数中传递
		typedef boost::shared_ptr<ip::tcp::socket> sock_pt;

	public:
		server(io_service& io): ios(io), acceptor(ios, ip::tcp::endpoint(ip::tcp::v4(), 6688))
		{
			start();
		}

		void start()
		{
			//智能指针, 是为了让socket对象能够被异步调用后还能使用
			sock_pt sock(new ip::tcp::socket(ios));

			//异步监听服务
			acceptor.async_accept(*sock, bind(&server::accept_handler, this, placeholders::error, sock));
		}

		void accept_handler(const boost::system::error_code& ec, sock_pt sock)
		{
			if (ec)
			{
				return;
			}

			cout << "clinet:";
			cout << sock->remote_endpoint().address() << endl;
			sock->async_write_some(buffer("hello asio"), bind(&server::write_handler, this, placeholders::error));

			//再次启动异步接受连接, 否则当完成数据发送后io_service将因为没有事件处理而结束运行。
			start();
		}

		//发送数据的回调函数
		void write_handler(const boost::system::error_code&)
		{
			cout << "send msg complete." << endl;	
		}
};

int main()
{
	try 
	{
		cout << "server start." << endl;
		io_service ios;

		server serv(ios);
		//启动异步调用事件处理循环
		ios.run();

	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
}
