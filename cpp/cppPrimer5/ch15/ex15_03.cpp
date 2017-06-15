class Quote {
	public:
		Quote() = default; // = default see § 7.1.4 (p. 264)
		Quote(const std::string &book, double sales_price): bookNo(book), price(sales_price) { }
		std::string isbn() const { return bookNo; }
		
		// returns the total sales price for the specified number of items
		// derived classes will override and apply different discount algorithms
		virtual double net_price(std::size_t n) const
		{ 
			return n*price; 
		}
		
		virtual ~Quote() = default; // dynamic binding for the destructor
	private:
		std::string bookNo; // ISBN number of this item
	protected:
		double price = 0.0; // normal, undiscounted price
};

double print_total(ostream& os, Quote const& item, size_t n) {
	double ret = item.net_price(n);
    
	os << "ISBN: " << item.isbn() << " # sold: " << n << " total due: " << ret << endl;
    return ret;
}



