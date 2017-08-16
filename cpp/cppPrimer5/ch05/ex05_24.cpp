#include <iostream>

using namespace std;

int main()
{
	int a, b;

	cin >> a;
	cin >> b;

	if (b == 0)
		throw "b is 0";

	cout << (a/b) << endl;
}
