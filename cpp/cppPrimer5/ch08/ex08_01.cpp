#include <iostream>

using namespace std;

istream& i_read(istream &in) 
{
    std::string buf;	
	while(in >> buf) 
		cout << buf;
	in.clear();

	return in;
}

int main(void)
{
	i_read(cin);	
}
