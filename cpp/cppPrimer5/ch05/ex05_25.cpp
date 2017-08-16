#include <iostream>
#include <stdexcept>

using namespace std;

int main()
{
	int a, b;

again:
	try {
		cin >> a;
		cin >> b;

		if (b == 0)
			throw runtime_error("b is 0");

		cout << (a/b) << endl;
	} catch (runtime_error err) {
		cout << err.what() << "\n Try again? Enter y or n" << endl;
		char c;
		cin >> c;

		if (!cin || c == 'y')
			goto again;
	}
}
