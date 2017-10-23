#include <boost/unordered_set.hpp>
#include <boost/assign.hpp>

using namespace boost;
using namespace std;

//template<class T>
template<typename T>
void hash_func() 
{
	using namespace boost::assign;

	T s = (list_of(1), 2, 3, 4, 5);

	for (typename T::iterator p = s.begin(); p != s.end(); ++p)
	{
			cout << *p << " ";
	}
	cout << endl;
	cout << s.size() << endl; 

	s.clear();
	cout << s.empty() << endl;
	s.insert(8);
	s.insert(45);
	cout << s.size() << endl;
	cout << *s.find(8) << endl;

	s.erase(45);
}

int main() 
{
	hash_func<unordered_set<int> >();
}
