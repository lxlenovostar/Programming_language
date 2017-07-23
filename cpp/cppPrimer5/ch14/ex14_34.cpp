#include <iostream>
#include <string>
#include <vector>
#include <algorithm>    // std::for_each

using namespace std;

class ex14_34 {
	public:
		int operator()(const int a, const int b, const int c) const 
		{ 
			return (a > 0) ? b : c;
		}
};

int main() 
{
	const ex14_34 test1;
    cout << test1(1,2,3) << endl;	
	
	ex14_34 test2;
    cout << test2(0,2,3) << endl;	

}
