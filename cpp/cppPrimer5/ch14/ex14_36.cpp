#include <iostream>
#include <string>
#include <vector>

using namespace std;

class WriteString {
	public:
		WriteString(istream &i = cin): is(i) {}
		string operator()() {
			std::string s;
			is >> s;
			return is ? s :  string();
		}
	
	private:
		istream &is;

};

int main(void)
{
	vector<string> vec;
	WriteString w;

	for (std::string tmp; ((tmp = w()) != "");) 
		vec.push_back(tmp);

	for (const auto& str : vec) 
		cout << str << "\t";
	
	cout << endl;
}
