#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

int main(void)
{
	string filename("test_8_4"); 
	ifstream input(filename);
	string tmp;
	vector<string> vec_str;	

	if (input) {
		while(input >> tmp) {
			cout << tmp << endl;
			vec_str.push_back(tmp);
		}
	} else {
		cerr << "couldn't open: " << filename;
	}
	return 0;
}
