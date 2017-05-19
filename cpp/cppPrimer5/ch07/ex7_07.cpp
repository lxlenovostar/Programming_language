//
//  ex7_03.cpp
//  Exercise 7.03
//

#include "ex7_06.h"
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

int main()
{
    Sales_data total;
    if (read(cin, total)) {
        Sales_data trans;
        while (read(cin, trans)) {
            if (total.isbn() == trans.isbn())
				total.combine(trans);
            else {
				print(cout, total) << endl;
                total = trans;
            }
        }
		print(cout, total) << endl;
    }
    else {
        std::cerr << "No data?!" << std::endl;
        return -1;
    }

    return 0;
}
