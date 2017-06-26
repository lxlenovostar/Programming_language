#include <boost/thread.hpp>
using namespace boost;

template<typename T>
class basic_atom: noncopyable 
{
	private:
		T n;
		//互斥量类型定义
		typedef mutex mutex_t;
		mutex_t mu;
	public:
		//构造函数
		basic_atom(T x = T()):n(x) {}

		T operator++()
		{
			mutex_t::scoped_lock lock(mu);
			return ++n;
		}

		//类型转换操作符定义
		operator T() 
		{
			return n;
		}
};
