//
// io_service_pool.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <stdexcept>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace http {
namespace server2 {

io_service_pool::io_service_pool(std::size_t pool_size)
  //: next_io_service_(0), next_context_(0)
  : next_io_service_(0)
{
  if (pool_size == 0)
    throw std::runtime_error("io_service_pool size is 0");

  std::string ws_crt_out_file  = "ws.crt";
  std::string ws_ca_out_file   = "ws.ca";
  std::string ws_key_out_file  = "ws.key";

  // Give all the io_services work to do so that their run() functions will not
  // exit until they are explicitly stopped.
  for (std::size_t i = 0; i < pool_size; ++i)
  {
    io_service_ptr io_service(new boost::asio::io_service);
    work_ptr work(new boost::asio::io_service::work(*io_service));
    io_services_.push_back(io_service);
    work_.push_back(work);
  }

  /*
  for (std::size_t i = 0; i < 100000; ++i)
  {
	context_ptr context(new ssl::context(ssl::context::sslv23));

  	context->use_certificate_chain_file(ws_crt_out_file);
  	context->use_private_key_file(ws_key_out_file, boost::asio::ssl::context::pem);
  	context->load_verify_file(ws_ca_out_file);
  	context->set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_client_once);
  	context->use_tmp_dh_file("dh512.pem");

	context_.push_back(context);
  }
  */

}

void io_service_pool::run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < io_services_.size(); ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, io_services_[i])));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void io_service_pool::stop()
{
  // Explicitly stop all io_services.
  for (std::size_t i = 0; i < io_services_.size(); ++i)
    io_services_[i]->stop();
}

boost::asio::io_service& io_service_pool::get_io_service()
{
  // Use a round-robin scheme to choose the next io_service to use.
  boost::asio::io_service& io_service = *io_services_[next_io_service_];
  ++next_io_service_;
  if (next_io_service_ == io_services_.size())
    next_io_service_ = 0;
  return io_service;
}

/*
boost::asio::ssl::context& io_service_pool::get_context()
{
  // Use a round-robin scheme to choose the next io_service to use.
  boost::asio::ssl::context& context = *context_[next_context_];
  ++next_context_;
  if (next_context_ == context_.size())
    next_context_ = 0;
  return context;
}
*/

} // namespace server2
} // namespace http
