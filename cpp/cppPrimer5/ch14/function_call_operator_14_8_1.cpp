#include <iostream>

using namespace std;

struct absInt {
	int operator() (int val) const {
		return val < 0 ? -val : val;
	}
};

int main() 
{
	int i = -42;
	// object that has a function-call operator
	absInt absObj;
	// passes i to absObj.operator()
	int ui = absObj(i);

	cout << ui << endl;
}
