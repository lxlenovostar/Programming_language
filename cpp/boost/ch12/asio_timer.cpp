#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp> 

//g++ -o test -g -Wall asio_timer.cpp -lboost_system

using namespace boost::asio;
using namespace std;

void print_b(const boost::system::error_code &e)
{
	cout << "hello world" << endl;
}

int main()
{
	io_service ios;

	//同步定时器
	/* 
	deadline_timer t(ios, boost::posix_time::seconds(2));
	//查看终止的绝对时间
	std::cout << t.expires_at() << std::endl;
	std::cout << t.expires_from_now() << std::endl;

	//wait()使用了操作系统的epoll等完成。
	t.wait();
	std::cout << "hello asio" << std::endl;
	*/
	
	deadline_timer t(ios, boost::posix_time::seconds(2));
	t.async_wait(print_b);
	cout << "it show before t expired." << endl;
	// run()函数会阻塞等待所有的操作完成。
	ios.run();
}
