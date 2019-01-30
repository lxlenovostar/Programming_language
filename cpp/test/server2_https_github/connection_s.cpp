//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_s.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "request_handler.hpp"
#include <glog/logging.h>

namespace http {
namespace server2 {

connection_s::connection_s(boost::asio::io_service& io_service, boost::asio::ssl::context& context, request_handler& handler)
  : socket_(io_service, context),
    request_handler_(handler)
{
}

/*
boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}
*/

void connection_s::start()
{
 /*
  socket_.async_read_some(boost::asio::buffer(buffer_),
      boost::bind(&connection_s::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  */

    socket_.async_handshake(boost::asio::ssl::stream_base::server,
        boost::bind(&connection_s::handle_handshake, shared_from_this(),
          boost::asio::placeholders::error));
}
 
void connection_s::handle_handshake(const boost::system::error_code& e)
{
	if (!e)
    {
  		socket_.async_read_some(boost::asio::buffer(buffer_),
      		boost::bind(&connection_s::handle_read, shared_from_this(),
        		boost::asio::placeholders::error,
        		boost::asio::placeholders::bytes_transferred));
    } else {
		//eof means that peer has cleanly closed the connection, so we need not to 
		//explicitly stop(shutdown) the connectoin.
		LOG(ERROR) << "Handshake error:" << e.message();
	}
}

void connection_s::handle_read(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  if (!e)
  {
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result)
    {
      request_handler_.handle_request(request_, reply_);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          boost::bind(&connection_s::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else if (!result)
    {
      reply_ = reply::stock_reply(reply::bad_request);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          boost::bind(&connection_s::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
      socket_.async_read_some(boost::asio::buffer(buffer_),
          boost::bind(&connection_s::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
}

void connection_s::handle_write(const boost::system::error_code& e)
{
  if (!e)
  {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    //socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	socket_.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the connection object will disappear and the object will be
  // destroyed automatically after this handler returns. The connection class's
  // destructor closes the socket.
}

} // namespace server2
} // namespace http
