#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "basic_atom.hpp"

using namespace boost;
using namespace std;

// -lboost_thread 

typedef basic_atom<int> atom_int;

//io流是共享资源，需要锁定。
mutex io_mu;

void printing(atom_int& x, const string& str)
{
	for (int i = 0; i < 5; ++i) 
	{
		mutex::scoped_lock lock(io_mu);
		cout << str << ++x << endl;
	}
}

void to_interrupt(atom_int& x, const string& str) 
{
	try 
	{
		for (int i = 0; i < 5; ++i) 
		{
			//this_thread::sleep(posix_time::seconds(1));
			mutex::scoped_lock lock(io_mu);
			cout << str << ++x << endl;
			//线程中断点
			this_thread::interruption_point();
		}
	}
	catch(thread_interrupted&) //捕获中断异常
	{
		cout << "thread_interrupted" << endl;
	}
}

int main()
{
	/*
	int y = int();
	std::cout << y << std::endl;
	*/

	atom_int x;

	/*
	//使用ref库传递引用
	thread t1(printing, ref(x), "hello");
	thread t2(printing, ref(x), "boost");

	t1.timed_join(posix_time::seconds(1));
	t2.join();

	//使用bind表达式
	thread t3(bind(printing, ref(x), "thread"));

	boost::this_thread::sleep(boost::posix_time::seconds(1));
	cout << "sleep 1 seconds" << endl;

	//子命名空间 this_thread
	this_thread::sleep(posix_time::seconds(2));
	cout << this_thread::get_id() << endl;
	this_thread::yield();
	*/

	/*
	//中断点
	thread t5(to_interrupt, ref(x), "hello");
	//this_thread::sleep(posix_time::seconds(3));
	t5.interrupt();
	t5.join();
	*/

	thread_group tg;
	tg.create_thread(bind(printing, ref(x), "c++"));
	tg.create_thread(bind(printing, ref(x), "boost"));
	tg.join_all();

	/*
	 * scoped_lock and scoped_try_lock
	 * */
	/*
	boost::mutex mu;
	boost::mutex::scoped_lock lock(mu);
	std::cout << "some operations" << std::endl;
	*/

	return 0;
}
