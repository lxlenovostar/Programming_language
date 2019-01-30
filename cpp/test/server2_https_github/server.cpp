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
#include <glog/logging.h>
#include <boost/bind.hpp>

namespace http {
namespace server2 {

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root, std::size_t io_service_pool_size)
  : io_service_pool_(io_service_pool_size),
    signals_(io_service_pool_.get_io_service()),
    acceptor_(io_service_pool_.get_io_service()),
	context_(new ssl::context(ssl::context::sslv23)),
    new_connection_(),
    new_connection_s_(),
    request_handler_(doc_root)
{
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
  /*
  boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
  */

  std::string ws_crt_out_file  = "server.crt";
  std::string ws_ca_out_file   = "server.ca";
  std::string ws_key_out_file  = "server.key";
    	
  context_->use_certificate_chain_file(ws_crt_out_file);
  context_->use_private_key_file(ws_key_out_file, boost::asio::ssl::context::pem);
  context_->load_verify_file(ws_ca_out_file);
  context_->set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_client_once);
  context_->use_tmp_dh_file("dh512.pem");
  
  boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());

  int one = 1;
  setsockopt(acceptor_.native_handle(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));

  //acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  //acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_port(true));
  //acceptor_.set_option(tcp::no_delay(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  //Test for http
  //start_accept();

  //Test for https
  start_accept_s();
}

void server::run()
{
  io_service_pool_.run();
}

void server::start_accept()
{
  new_connection_.reset(new connection(
        io_service_pool_.get_io_service(), request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&server::handle_accept, this,
        boost::asio::placeholders::error));
}

void server::start_accept_s()
{
 /*
  new_connection_.reset(new connection(
        io_service_pool_.get_io_service(), request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&server::handle_accept, this,
        boost::asio::placeholders::error));
	*/

	
    new_connection_s_.reset(new connection_s(io_service_pool_.get_io_service(), *context_, request_handler_));
    //new_connection_s_.reset(new connection_s(io_service_pool_.get_io_service(), io_service_pool_.get_context(), request_handler_));
    acceptor_.async_accept(new_connection_s_->socket().lowest_layer(),
    		boost::bind(&server::handle_accept_s, this,
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

void server::handle_accept_s(const boost::system::error_code& e)
{
    try
    {
	    if (!e)
    	{
			//new_connection_s_->socket().lowest_layer().set_option(tcp::no_delay(true));
    		new_connection_s_->start();
    	}
    	else
    	{
    		LOG(FATAL) << "Accept error:" << boost::system::system_error(e).what();
    		if ( e == boost::asio::error::no_descriptors ) 
    		{
    			exit(1);
    		}
    	}
    	start_accept_s();
    }
     catch (const std::exception& e)
	{
		LOG(FATAL) << "Error occurs(start_write): " << e.what();
		return;
	}
}

void server::handle_stop()
{
  io_service_pool_.stop();
}

} // namespace server2
} // namespace http
