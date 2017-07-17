#include <iostream>
#include <string>
#include <vector>
#include <algorithm>    // std::for_each

using namespace std;

class PrintString {
	public:
		PrintString(ostream &o = cout, char c = ' '):os(o), sep(c) 
		{ }
				
		// 默认情况下，this类型是指向类类型非常量版本的常量指针。
		// 这里的第二个const作用是修改隐式this指针的类型。
		// 常量对象不能调用普通的成员函数。
		// 常量对象，以及常量对象的引用或指针都只能调用常量成员函数。
		void operator()(const string &s) const 
		{ 
			os << s << sep;
		}

		private:
			ostream &os; // stream on which to write
			char sep; // character to print after each output
};

int main() 
{
	string s = "hi world";
	
	const PrintString printer;
	printer(s);	
	const PrintString errors(cerr, '\n');
	errors(s);

	vector<string> vs; 
	vs.push_back("abc");
	vs.push_back("efg");
	vs.push_back("xyz");
	for_each(vs.begin(), vs.end(), PrintString(cerr, '\n'));
}
