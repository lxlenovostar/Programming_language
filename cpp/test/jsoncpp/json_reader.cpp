#include <iostream>
#include <jsoncpp/json/json.h> // or something

using namespace std;

int main() {
	Json::Value val;
	Json::Reader reader;
    bool b = reader.parse(cin, val);

	if (!b)
    	cout << "Error: " << reader.getFormattedErrorMessages();
	else
        cout << val;
}
