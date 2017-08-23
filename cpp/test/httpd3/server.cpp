//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace http {
namespace server3 {

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root, std::size_t thread_pool_size)
  : thread_pool_size_(thread_pool_size),
    signals_(io_service_),
    acceptor_(io_service_),
    new_connection_(),
    request_handler_(doc_root)
{
  // SIGINT
  // When the user types the terminal interrupt character (usually Control-C), the
  // terminal driver sends this signal to the foreground process group. The
  // default action for this signal is to terminate the process.
  //
  // SIGTERM
  // This is the standard signal used for terminating a process and is the default
  // signal sent by the kill and killall commands. Users sometimes explicitly
  // send the SIGKILL signal to a process using kill –KILL or kill –9. However,
  // this is generally a mistake. A well-designed application will have a handler
  // for SIGTERM that causes the application to exit gracefully, cleaning up 
  // temporary files and releasing other resources beforehand. Killing a process
  // with SIGKILL bypasses the SIGTERM handler. Thus, we should always first
  // attempt to terminate a process using SIGTERM, and reserve SIGKILL as a last
  // resort for killing runaway processes that don’t respond to SIGTERM.
  //
  // SIGQUIT
  // When the user types the quit character (usually Control-\) on the keyboard,
  // this signal is sent to the foreground process group. By default, this signal
  // terminates a process and causes it to produce a core dump, which can then
  // be used for debugging. Using SIGQUIT in this manner is useful with a program 
  // that is stuck in an infinite loop or is otherwise not responding. By
  // typing Control-\ and then loading the resulting core dump with the gdb
  // debugger and using the backtrace command to obtain a stack trace, we can
  // find out which part of the program code was executing. ([Matloff, 2008]
  // describes the use of gdb.)
  //
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&server::handle_stop, this));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
}

void server::run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void server::start_accept()
{
  new_connection_.reset(new connection(io_service_, request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&server::handle_accept, this,
        boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e)
{
  if (!e)
  {
    new_connection_->start();
  }

  start_accept();
}

void server::handle_stop()
{
  io_service_.stop();
}

} // namespace server3
} // namespace http
