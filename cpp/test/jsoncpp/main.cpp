#include <string>
#include <iostream>
#include "include/json/json.h"  

// g++ -g -Wall -o test_json main.cpp lib_json/*.cpp -I./include  

using namespace std;  

int main()   
{  
	/*
	std::cout <<  "example 1:" << std::endl;  
	std::string test = "{\"id\":1,\"name\":\"hello\"}";  
    Json::Reader reader;  
    Json::Value value;  
    if (reader.parse(test, value)) {  
    	int id = value["id"].asInt();  
		std::string name = value["name"].asString();  
		std::cout << id << " " << name << std::endl;  
	} else {  
		std::cout << "parse error" << std::endl;  
	}  
	*/

	std::cout << "example 2:" << std::endl;  
  	string test = "{\"array\":[{\"id\":1,\"name\":\"hello\"},{\"id\":2,\"name\":\"world\"}]}";  
    Json::Reader reader;  
  	Json::Value value;  

    if (reader.parse(test, value)) {  
		const Json::Value arrayObj = value["array"];  
	    for (size_t i = 0; i < arrayObj.size(); i++) {  
			int id = (arrayObj[i])["id"].asInt();  
		    string name = (arrayObj[i])["name"].asString();  
			std::cout << id << " " << name << std::endl;  
		} 
	}
	else {  
		std::cout << "parse error" << std::endl;  
	}  
	
}
