#include <iostream>

using namespace std;

int main(void)
{
	auto old_state = cin.rdstate();
	cin.clear(cin.rdstate() & ~cin.failbit & ~cin.badbit);
	cin.setstate(old_state);
}
