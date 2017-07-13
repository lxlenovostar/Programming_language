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
  	Json::Value root;  

    if (reader.parse(test, root)) {  
		//const Json::Value arrayObj = value["array"];  
		//std::cout << arrayObj.size() << std::endl;
		/*
	    for (size_t i = 0; i < arrayObj.size(); i++) {  
			int id = arrayObj[i]["id"].asInt();  
		    string name = arrayObj[i]["name"].asString();  
			std::cout << id << " " << name << std::endl;  
		} 
		*/
		/*
		if (root["0017"].empty() == false)
			std::cout << root["array"].size() << std::endl;
		else 
			std::cout << "fuck" << std::endl;
		*/

		std::cout << root["array"].type() << std::endl;

		Json::Value::Members mem = root.getMemberNames();
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++) {
			std::string newKey = *iter;
			std::cout << newKey << iter->c_str()  << std::endl;
		}
		/*
		for (Json::Value::iterator it = root["array"].begin(); it != root["array"].end(); ++it) {
			std::cout << (*it)["id"].asInt() << " " << (*it)["name"].asString() << std::endl;  
		}
		*/
	}
	else {  
		std::cout << "parse error" << std::endl;  
	}  
	
}
