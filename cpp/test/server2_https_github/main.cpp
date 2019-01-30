//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"

// g++ -g -Wall -o https_server2  *.cpp   -I/usr/local/include/ -Wl,-Bstatic -lglog -lunwind  -lboost_system -lboost_thread -Wl,-Bdynamic -lpthread  -lcrypto -lssl
// g++ -g -Wall -o https_server2  *.cpp   -I/usr/local/include/ -I/usr/local/openssl-1.1.0j/include -L/usr/local/openssl-1.1.0j/lib -I/usr/local/boost_1_64_0/include /usr/local/boost_1_64_0/lib/libboost_thread.a /usr/local/boost_1_64_0/lib/libboost_system.a   -Wl,-Bstatic -lglog -lunwind   -Wl,-Bdynamic -lpthread -lcrypto -lssl  -ldl -lrt

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 5)
    {
      std::cerr << "Usage: http_server <address> <port> <threads> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 1 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 1 .\n";
      return 1;
    }

    // Initialise the server.
    std::size_t num_threads = boost::lexical_cast<std::size_t>(argv[3]);
    http::server2::server s(argv[1], argv[2], argv[4], num_threads);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
