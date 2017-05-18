//  ex7_02.h
//  Exercise 7.2

#ifndef CP5_ex7_04_h
#define CP5_ex7_04_h
#include <string>

struct Person {
    std::string name;
    std::string address;
   
  	public:	
	std::string getname() const { return name; };
	std::string getaddress() const { return address; };
};

#endif

// Should these functions be const?
//
// Yes, A const following the parameter list indicates that this is a pointer to
// const.
// These functions my read but not write to the data members of the objects on
// which it is called.
