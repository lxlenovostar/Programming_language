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
#include <boost/date_time/posix_time/posix_time.hpp>

void print(const boost::system::error_code& /*e*/)
{
  std::cout << "Hello, world!\n";
}

int main()
{
  boost::asio::io_service io;

  boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
  t.async_wait(&print);

  // The asio library provides a guarantee that callback handlers 
  // will only be called from threads that are currently calling 
  // io_service::run(). Therefore unless the io_service::run() 
  // function is called the callback for the asynchronous wait 
  // completion will never be invoked.
  // 
  // The io_service::run() function will also continue to run while 
  // there is still "work" to do. In this example, the work is the 
  // asynchronous wait on the timer, so the call will not return until 
  // the timer has expired and the callback has completed.
  //
  // It is important to remember to give the io_service some work to do 
  // before calling io_service::run(). For example, if we had omitted the 
  // above call to deadline_timer::async_wait(), the io_service would not 
  // have had any work to do, and consequently io_service::run() would have '
  // returned immediately.
  io.run();

  return 0;
}
