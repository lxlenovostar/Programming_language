//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "request_handler.hpp"

namespace http {
namespace server3 {

connection::connection(boost::asio::io_service& io_service,
    request_handler& handler)
  : strand_(io_service),
    socket_(io_service),
    request_handler_(handler)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

void connection::start()
{
  // placeholders::error: An argument placeholder, for use with boost::bind(), 
  // that corresponds to the error argument of a handler for any of the 
  // asynchronous functions.
  //
  // placeholders::bytes_transferred: An argument placeholder, for use with 
  // boost::bind(), that corresponds to the bytes_transferred argument of 
  // a handler for asynchronous functions such as 
  // boost::asio::basic_stream_socket::async_write_some or boost::asio::async_write.
  //
  // shared_from_this()在一个类中需要传递类对象本身shared_ptr的地方使用
  // shared_from_this函数来获得指向自身的shared_ptr，它是
  // enable_shared_from_this<T>的成员函数，返回shared_ptr<T>。 
  //
  socket_.async_read_some(boost::asio::buffer(buffer_),
      strand_.wrap(
        boost::bind(&connection::handle_read, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred)));
}

// 准确的字节数通过 std::size_t 类型的参数 bytes_transferred 给出。 
void connection::handle_read(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  if (!e)
  {
    boost::tribool result;
	// ignore对象用来忽略无用的tuple元素，
	// ignore位于boost::tuples名字空间下。
	//
	//  Tier 的特殊之处在于它包含的所有元素都是引用类型的。 
	//  它可以通过构造函数 boost::tie() 来创建。
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result)
    {
	  // 处理具体的请求 
      request_handler_.handle_request(request_, reply_);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&connection::handle_write, shared_from_this(),
              boost::asio::placeholders::error)));
    }
    else if (!result)
    {
      reply_ = reply::stock_reply(reply::bad_request);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&connection::handle_write, shared_from_this(),
              boost::asio::placeholders::error)));
    }
    else
    {
      socket_.async_read_some(boost::asio::buffer(buffer_),
          strand_.wrap(
            boost::bind(&connection::handle_read, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred)));
    }
  }

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const boost::system::error_code& e)
{
  if (!e)
  {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the connection object will disappear and the object will be
  // destroyed automatically after this handler returns. The connection class's
  // destructor closes the socket.
}

} // namespace server3
} // namespace http
