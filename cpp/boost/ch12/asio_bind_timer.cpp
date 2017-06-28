#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp> 
#include <boost/function.hpp> 
#include <boost/bind.hpp> 

//g++ -o test -g -Wall asio_timer.cpp -lboost_system

using namespace boost::asio;
using namespace std;

class a_timer
{
	private:
		int count, count_max;
		boost::function<void()> f;
		deadline_timer t;
	public:
		template<typename F>
		a_timer(io_service &ios, int x, F func):
				count(0), count_max(x), f(func), t(ios, boost::posix_time::millisec(500))
		{
			//需要在run()之前注册回调函数
			//call_func是a_timer的一个成员函数，因此我们需要绑定this指针，同时我们还使用了
			//asio下子名字空间placeholders下的一个占位符error.
			t.async_wait(bind(&a_timer::call_func, this, placeholders::error));
		}

		void call_func(const boost::system::error_code& e)
		{
			if (count >= count_max) 
			{
				return;
			}
			++count;
			f();

			//如果计数器没有达到上限就调用function对象f,然后重新设置
			//定时器的终止时间，再次异步等待被调用，从而达到返回执行的目的。
			t.expires_at(t.expires_at() + boost::posix_time::millisec(500));
			t.async_wait(bind(&a_timer::call_func, this, placeholders::error));
		}
};

void print1() 
{
	cout << "hello asio" << endl;
}

void print2() 
{
	cout << "hello boost" << endl;
}

int main()
{
	io_service ios;
	a_timer at1(ios, 5, print1);
	a_timer at2(ios, 5, print2);
	ios.run();
}
