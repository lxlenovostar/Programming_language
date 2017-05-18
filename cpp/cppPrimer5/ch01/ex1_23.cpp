#include <iostream>
#include "../include/Sales_item.h"

int main()
{
    Sales_item current;
	int count = 1;
    if (std::cin >> current) {
        Sales_item trans;
        while (std::cin >> trans) {
            if (current.isbn() == trans.isbn())
                ++count;
            else {
                std::cout << current <<  " " << count << std::endl;
                current = trans;
				count = 1;
            }
        }
        std::cout << current <<  " " << count << std::endl;
    }
    else {
        std::cerr << "No data?!" << std::endl;
        return -1;
    }

    return 0;
}
