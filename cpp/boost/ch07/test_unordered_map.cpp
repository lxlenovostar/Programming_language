#include <boost/unordered_map.hpp>
#include <boost/assign.hpp>
#include <boost/typeof/typeof.hpp>
using namespace boost;
using namespace std;

int main() 
{
	using namespace boost::assign;

	unordered_map<int, string> um = map_list_of(1, "one")(2, "two")(3,"three");

	um.insert(make_pair(10, "ten"));
	cout << um[10] << endl;
	um[11] = "eleven";
	um[15] = "fifteen";

	BOOST_AUTO(p, um.begin());
	for(p; p != um.end(); ++p)
		cout << p->first << "-" << p->second << ",";
	cout << endl;

	cout << um.size() << endl;
}
