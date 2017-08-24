//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class printer
{
public:
  printer(boost::asio::io_service& io)
    : timer_(io, boost::posix_time::seconds(1)),
      count_(0)
  {
	//  all non-static class member functions have an implicit 
	//  this parameter, we need to bind this to the function.
	//
	//  You will note that the boost::asio::placeholders::error 
	//  placeholder is not specified here, as the print member 
	//  function does not accept an error object as a parameter.
    //timer_.async_wait(boost::bind(&printer::print, this, boost::asio::placeholders::error));
    timer_.async_wait(boost::bind(&printer::print, this, _1));
  }

  ~printer()
  {
    std::cout << "Final count is " << count_ << "\n";
  }

  void print(const boost::system::error_code& e)
  {
    //std::cout << "e is:" <<  boost::system::system_error(e).what()  << "\n";
    if (count_ < 5)
    {
      std::cout << count_ << "\n";
      ++count_;

      timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
      //timer_.async_wait(boost::bind(&printer::print, this, boost::asio::placeholders::error));
      timer_.async_wait(boost::bind(&printer::print, this, _1));
    }
  }

private:
  boost::asio::deadline_timer timer_;
  int count_;
};

int main()
{
  boost::asio::io_service io;
  printer p(io);
  io.run();

  return 0;
}
