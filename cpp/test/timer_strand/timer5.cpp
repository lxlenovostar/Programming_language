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
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// The single threaded approach is usually the best place to 
// start when developing applications using asio. The downside 
// is the limitations it places on programs, particularly servers, including:
//
// 1. Poor responsiveness when handlers can take a long time to complete.
// 2. An inability to scale on multiprocessor systems.

class printer
{
public:
  printer(boost::asio::io_service& io)
    : strand_(io),
      timer1_(io, boost::posix_time::seconds(1)),
      timer2_(io, boost::posix_time::seconds(1)),
      count_(0)
  {
    // The strand::wrap() function returns a new handler 
	// that automatically dispatches its contained handler 
	// through the boost::asio::strand object. By wrapping 
	// the handlers using the same boost::asio::strand, we 
	// are ensuring that they cannot execute concurrently.
    timer1_.async_wait(strand_.wrap(boost::bind(&printer::print1, this)));
    timer2_.async_wait(strand_.wrap(boost::bind(&printer::print2, this)));
  }

  ~printer()
  {
    std::cout << "Final count is " << count_ << "\n";
  }

  void print1()
  {
    if (count_ < 10)
    {
      std::cout << "Timer 1: " << count_ << "\n";
	  std::string id = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	  std::cout << "Thread id: " << id << "\n";
      ++count_;

      timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
      timer1_.async_wait(strand_.wrap(boost::bind(&printer::print1, this)));
    }
  }

  void print2()
  {
    if (count_ < 10)
    {
      std::cout << "Timer 2: " << count_ << "\n";
	  std::string id = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	  std::cout << "Thread id: " << id << "\n";
      ++count_;

      timer2_.expires_at(timer2_.expires_at() + boost::posix_time::seconds(1));
      timer2_.async_wait(strand_.wrap(boost::bind(&printer::print2, this)));
    }
  }

private:
  // An boost::asio::strand guarantees that, for those handlers 
  // that are dispatched through it, an executing handler will 
  // be allowed to complete before the next one is started. This 
  // is guaranteed irrespective of the number of threads that are 
  // calling io_service::run(). Of course, the handlers may still 
  // execute concurrently with other handlers that were not dispatched 
  // through an boost::asio::strand, or were dispatched through a 
  // different boost::asio::strand object.
  boost::asio::strand strand_;
  boost::asio::deadline_timer timer1_;
  boost::asio::deadline_timer timer2_;
  int count_;
};

int main()
{
  // The main function now causes io_service::run() to be called from 
  // two threads: the main thread and one additional thread. This is 
  // accomplished using an boost::thread object.
  boost::asio::io_service io;
  printer p(io);
  boost::thread t(boost::bind(&boost::asio::io_service::run, &io));
  io.run();
  t.join();

  return 0;
}
