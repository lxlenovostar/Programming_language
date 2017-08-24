#include <boost/bind.hpp>
#include <iostream>

using namespace std;
using namespace boost;

int f(int a, int b)
{
	return a + b;
}

int g(int a, int b, int c)
{
	return a + b * c;
}

// 函数指针定义
typedef int (*f_type)(int, int);
typedef int (*g_type)(int, int, int);

struct demo 
{
	int f(int a, int b)
	{
		return a + b;
	}
};

int main()
{
	int x = 3;
	int y = 4;
	int z = 5;

	f_type pf = f;
	g_type pg = g;

	std::cout << bind(f, 1, 2)() << std::endl;	
	std::cout << bind(g, 1, 2, 3)() << std::endl;
	std::cout << bind(f, _1, 9)(x) << std::endl;
	std::cout << bind(f, _1, _1)(x, y) << std::endl;

	// 绑定函数指针
	std::cout << bind(pf, _1, 9)(x) << std::endl; 	//(*pf)(x,9)
	std::cout << bind(pg, _3, _2, _2)(x, y, z) << std::endl; 	//(*pg)(z,y,y)

	// 类的实例对象和引用
	demo a, &ra=a;		
	// 指针
	demo *p = &a;

	std::cout << bind(&demo::f, a, _1, 20)(10) << std::endl;
	std::cout << bind(&demo::f, ra, _2, _1)(10, 20) << std::endl;
	std::cout << bind(&demo::f, p, _1, _2)(10, 20) << std::endl;


}



