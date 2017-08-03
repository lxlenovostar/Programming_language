#include <iostream> 
#include <fstream> 
#include <jsoncpp/json/json.h> // or something 

using namespace std; 

int main() 
{ 
	Json::Reader reader; 
	Json::Value val; 
	
	reader.parse("{\"one\":1,\"two\":2,\"three\":3}", val); 
	
	// We cannot use const_iterator below, since val.begin() and val.end() are 
	// non-const iterators, and there is no conversion. 
	// (Unless we use new version.) 
	for(Json::Value::iterator it=val.begin(); it!=val.end(); ++it) 
		cout << (*it).asInt() << '\n'; 
}
