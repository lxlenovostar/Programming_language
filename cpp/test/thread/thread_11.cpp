#include <iostream>
#include <thread>

// g++ -g -Wall -pthread thread_11.cpp -std=c++11

void fun()
{
	std::cout << "A new thread!" << std::endl;
}

int main(void)
{
	//std::thread t([]{ std::cout << "start" << std::endl;});
	std::thread t(fun);
    t.join();
    std::cout << "Main thread!" << std::endl;
}
