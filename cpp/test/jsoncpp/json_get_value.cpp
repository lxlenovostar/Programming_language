#include <iostream> 
#include <jsoncpp/json/json.h> // or something 

using namespace std; 

int main() 
{ 
	Json::Value val; 
	cin >> val; 
	
	switch (val.type()) 
	{ 
		case Json::nullValue: cout << "null\n"; break; 
		case Json::intValue: cout << "int " << val.asLargestInt() << "\n"; break; 
		case Json::uintValue: cout << "uint " << val.asLargestUInt() << "\n"; break; 
		case Json::realValue: cout << "real " << val.asDouble() << "\n"; break; 
		case Json::stringValue: cout << "string " << val.asString() << "\n"; break; 
		/* 
		 * -or- 
		 *  case Json::stringValue: { 
		 *  	const char *begin; 
		 *  	const char *end; 
		 *  	val.getString(&begin, &end); 
		 *  	cout << "string of length " << end - begin << "\n"; 
		 *  	} 
		 *  break; 
		 **/ 
		case Json::booleanValue: cout << "boolean " << val.asBool() << "\n"; break; 
		case Json::arrayValue: cout << "array of length " << val.size() << "\n"; break; 
		case Json::objectValue: cout << "object of length " << val.size() << "\n"; break; 
		default: cout << "wrong type\n"; break; 
	} 
}
