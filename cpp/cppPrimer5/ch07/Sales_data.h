#include <string>

class Sales_data {
	// friend declarations for nonmember Sales_data operations added
	friend Sales_data add(const Sales_data&, const Sales_data&);
	friend std::istream &read(std::istream&, Sales_data&);
	friend std::ostream &print(std::ostream&, const Sales_data&);
public:
	Sales_data() = default;
	Sales_data(const std::string &s, unsigned n, double p): bookNo(s), units_sold(n), revenue(p*n) 
	{ 
	}

	Sales_data(const std::string &s): bookNo(s) 
	{ 
	}

	Sales_data(std::istream&);

	// new members: operations on Sales_data objects
	std::string isbn() const 
	{ 
		return bookNo; 
	}

	Sales_data &combine(const Sales_data&);
private:	
	double avg_price() const 
	{
		return units_sold ? revenue/units_sold : 0;
	}

	// data members are unchanged from § 2.6.1 (p. 72)
	std::string bookNo;
	unsigned units_sold = 0;
	double revenue = 0.0;
};

// nonmember Sales_data interface functions
Sales_data add(const Sales_data&, const Sales_data&);
std::ostream &print(std::ostream&, const Sales_data&);
std::istream &read(std::istream&, Sales_data&);
