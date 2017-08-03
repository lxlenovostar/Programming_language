#include <iostream> 
#include <jsoncpp/json/json.h> // or something 

using namespace std; 

int main() 
{ 
	Json::Value val; 
	cin >> val; 
	
	switch (val.type()) { 
		case Json::nullValue: cout << "nullValue\n"; break; 
		case Json::intValue: cout << "intValue\n"; break; 
		case Json::uintValue: cout << "uintValue\n"; break; 
		case Json::realValue: cout << "realValue\n"; break; 
		case Json::stringValue: cout << "stringValue\n"; break; 
		case Json::booleanValue: cout << "booleanValue\n"; break; 
		case Json::arrayValue: cout << "arrayValue\n"; break; 
		case Json::objectValue: cout << "objectValue\n"; break; 
		default: cout << "wrong type\n"; break; 
	} 
}
