#include <iostream>
#include "../include/Sales_item.h"

int main()
{
	Sales_item item1, item2;
	std::cin >> item1 >> item2; // read a pair of transactions

	if (item1.isbn() == item2.isbn()) {
		std::cout << item1 + item2 << std::endl; // print their sum
		return 0;
	} else { 
		std::cerr << "not same isbn." << "\n";
		return -1;
	}
}
