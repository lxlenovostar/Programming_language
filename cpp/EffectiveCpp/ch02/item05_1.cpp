#include <string>
#include <iostream>

using namespace std;

template<class T>
class NamedObject {
public:
		NamedObject(const char* name, const T& value) 
		{
			objectValue = value;
			nameValue = name;
		}	
		//NamedObject(const std::string& name, const T& value);
	
		std::string get_nameValue()
		{
			return nameValue;
		}

		T get_objectValue()
		{
			return objectValue;
		}
private:
		std::string nameValue;
		T objectValue;
};

int main(void)
{
	NamedObject<int> no1("Smallest Prime Number", 2);
	cout << no1.get_nameValue() << no1.get_objectValue() << endl;
	NamedObject<int> no2(no1);
	cout << no2.get_nameValue() << no2.get_objectValue() << endl;
}
