#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace boost;
using namespace std;

int main()
{
	shared_ptr<int> spi(new int);
	assert(spi);
	cout << *spi << endl;
	*spi = 253;
	cout << *spi << endl;
	shared_ptr<string> sps(new string("smart"));
	cout << *sps << endl;
	assert(sps->size() == 5);

	/*
	 * shared_ptr 支持比较运算，因此shared_ptr可以被用于标准关联容器(set和map)
	 * */
	typedef shared_ptr<string> sp_t;
	map<sp_t, int> m;
	sp_t sp(new string("one"));
	m[sp] = 111;

	/*
	 * 学习 static_cast
	 * */
	int i = 1, j = 2;
	double slope = static_cast<double>(j) / i;

	void *p = &slope;	
	double *dp = static_cast<double *>(p);
	cout << *dp << endl;

	/*
	 * 学习 shared_ptr
	 * */
	shared_ptr<int> sp1(new int(10));
	assert(sp1.unique());
	shared_ptr<int> sp2 = sp1;
	assert(sp1 == sp2 && sp1.use_count() == 2);
	*sp1 = 100;
	assert(*sp1 == 100);
	sp1.reset();
	assert(!sp1);
	assert(sp2.use_count() == 1);
	sp2.reset();
	assert(!sp2);

	/*
	 * shared_ptr 工厂函数
	 * */
	shared_ptr<string> sp3 = make_shared<string>("make_shared");
	shared_ptr<vector<int> > spv = make_shared<vector<int> >(10, 2);
	assert(spv->size() == 10);

	/*
	 * shared_ptr 应用于 标准容器
	 * */
	typedef vector<shared_ptr<int> > vs;
	vs v(10);

	int i_v = 0;
	for (vs::iterator pos = v.begin(); pos != v.end(); ++pos)
	{
		(*pos) = make_shared<int>(++i_v);
		cout << *(*pos) << ", ";
	}
	cout << endl;

	shared_ptr<int> p1 = v[9];
	*p1 = 100;
	cout << *v[9] << endl;

	return 0;
}
