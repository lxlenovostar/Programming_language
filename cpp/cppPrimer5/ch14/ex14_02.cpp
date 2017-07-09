//
//  ex14_02.cpp
//  Exercise 14.2
//
//  Created by pezy on 3/9/15.
//  Copyright (c) 2014 pezy. All rights reserved.
//
//  @Brief implementation of class Sales_data
//  @See ex14_02.h

#include "ex14_02.h"

Sales_data::Sales_data(std::istream& is) : Sales_data()
{
    //is >> *this;

	//read函数的作用是从is中读取一条交易信息然后存入this对象中
	read(is, *this);
}

Sales_data& Sales_data::operator+=(const Sales_data& rhs)
{
    units_sold += rhs.units_sold;
    revenue += rhs.revenue;
    return *this;
}

std::istream& operator>>(std::istream& is, Sales_data& item)
{
    double price = 0.0;
    is >> item.bookNo >> item.units_sold >> price;
    if (is)
        item.revenue = price * item.units_sold;
    else
        item = Sales_data();
    return is;
}

/*
// input transactions contain ISBN, number of copies sold, and sales price
istream &read(istream &is, Sales_data &item)
{
	double price = 0;
	is >> item.bookNo >> item.units_sold >> price;
	item.revenue = price * item.units_sold;
	return is;
}

ostream &print(ostream &os, const Sales_data &item)
{
	os << item.isbn() << " " << item.units_sold << " "
	<< item.revenue << " " << item.avg_price();
	return os;
}
*/

//第一个形参是一个非常量ostream对象的引用。之所以ostream是非常量是因为
//向流写入内容会改变其状态；而该形参是引用是因为我们无法直接复制一个ostream
//对象。
//第二个形参一般来说是一个常量的引用，该常量是我们想要打印的类类型。
//第二个形参是引用的原因是我们希望避免复制是实参；而之所以该形参是常量
//是因为打印对象不会改变对象的内容。
//为了与其他输出运算符保持一致，operator<<一般要返回它的ostream形参。
std::ostream& operator<<(std::ostream& os, const Sales_data& item)
{
    os << item.isbn() << " " << item.units_sold << " " << item.revenue << " "
       << item.avg_price();
    return os;
}

Sales_data operator+(const Sales_data& lhs, const Sales_data& rhs)
{
    Sales_data sum = lhs;
    sum += rhs;
    return sum;
}
