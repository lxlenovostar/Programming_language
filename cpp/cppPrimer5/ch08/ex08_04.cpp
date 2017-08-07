#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

int main(void)
{
	string filename("test_8_4"); 
	ifstream input(filename.c_str());
	string tmp_line;
	vector<string> vec_str;	

	if (input) {
		while(getline(input, tmp_line)) {
			cout << tmp_line;
			vec_str.push_back(tmp_line);
		}
	} else {
		cerr << "couldn't open: " << filename;
	}
	return 0;
}
