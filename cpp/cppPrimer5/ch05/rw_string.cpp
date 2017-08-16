#include <string>
#include <iostream>

using namespace std;

int main()
{
	/*
	string s1, s2;
	
	//string对象会自动忽略开头的空白（空格符、换行符、制表符等）并从
	//第一个真正的字符开始读起，直到遇见下一个空白为止。
	cin >> s1 >> s2;
	cout << s1 << s2 << endl;
	*/

	/*	
	string word;
	//while(cin >> word) 
	while(getline(cin, word)) 
		cout << word << endl;
	*/

	string test("hello world");
	cout << test << endl;
	cout << test.c_str() << endl;

	return 0;

}
