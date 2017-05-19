//  ex7_02.h
//  Exercise 7.2

#ifndef CP5_ex7_04_h
#define CP5_ex7_04_h
#include <string>

struct Person {
    std::string name;
    std::string address;
   
  	public:	
	std::string getName() const { return name; };
	std::string getAddress() const { return address; };
};

// nonmember functions
std::istream& read(std::istream& is, Person& p)
{
    is >> p.name >> p.address;
    return is;
}

std::ostream& print(std::ostream& os, const Person& p)
{
    os << p.getName() << " " << p.getAddress(); 
    return os;
}


#endif

// Should these functions be const?
//
// Yes, A const following the parameter list indicates that this is a pointer to
// const.
// These functions my read but not write to the data members of the objects on
// which it is called.
