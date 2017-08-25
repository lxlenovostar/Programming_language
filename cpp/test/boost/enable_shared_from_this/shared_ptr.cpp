#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class Y: public boost::enable_shared_from_this<Y>
{
	public:
		boost::shared_ptr<Y> f()
	    {
	    	return shared_from_this();
		}
};

int main()
{
	boost::shared_ptr<Y> p(new Y);
	boost::shared_ptr<Y> q = p->f();
    assert(p == q);
    assert(!(p < q || q < p)); // p and q must share ownership
}
