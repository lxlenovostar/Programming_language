#include <iostream>
#include <jsoncpp/json/json.h> // or something

using namespace std;
 
int main() {
	Json::Value val;
	try 
	{
    	cin >> val;
    	cout << val;
	} 
	catch (Json::RuntimeError err)
	{
		cout << "error: \n" << err.what()  << endl;
	}
}
