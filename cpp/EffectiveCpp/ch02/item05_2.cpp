#include <string>
#include <iostream>

using namespace std;

template<class T>
class NamedObject {
public:
		NamedObject(const std::string& name, const T& value)
		{
			nameValue = name;
			objectValue = value;
		}
	
		std::string get_nameValue()
		{
			return nameValue;
		}

		T get_objectValue()
		{
			return objectValue;
		}
private:
		std::string& nameValue;
		const T objectValue;
};

int main(void)
{
	std::string newDog("Persephone");
	std::string oldDog("Satch");

	NamedObject<int> p(newDog, 2);
	NamedObject<int> s(oldDog, 36);
	p = s;
}
