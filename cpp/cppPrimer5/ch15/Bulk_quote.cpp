class Bulk_quote : public Quote { // Bulk_quote inherits from Quote
		Bulk_quote() = default;
		Bulk_quote(const std::string&, double, std::size_t, double);
		Bulk_quote(const std::string& book, double p, std::size_t qty, double disc) :
				Quote(book, p), min_qty(qty), discount(disc) 
		{ }

		// as before
		// };
		// overrides the base version in order to implement the bulk purchase discount policy
		double net_price(std::size_t) const override;
	private:
		std::size_t min_qty = 0; // minimum purchase for the discount to apply
		double discount = 0.0; // fractional discount to apply
};

// if the specified number of items are purchased, use the discounted price
double Bulk_quote::net_price(size_t cnt) const
{
	if (cnt >= min_qty)
		return cnt * (1 - discount) * price;
	else
		return cnt * price;
}

