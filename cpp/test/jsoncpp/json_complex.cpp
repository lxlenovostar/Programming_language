#include <iostream> 
#include <jsoncpp/json/json.h> // or something 

using namespace std; 

int main() 
{ 
	// create the characters array 
	Json::Value ch; 
	
	ch[0]["name"] = "Jabberwock"; 
	ch[0]["chapter"] = 1; 
	ch[1]["name"] = "Cheshire Cat"; 
	ch[1]["chapter"] = 6; 
	ch[2]["name"] = "Mad Hatter"; 
	ch[2]["chapter"] = 7; 
	
	// create the main object 
	Json::Value val; 
	val["book"] = "Alice in Wonderland"; 
	val["year"] = 1865; 
	val["characters"] = ch; 
	cout << val << '\n'; 
}
