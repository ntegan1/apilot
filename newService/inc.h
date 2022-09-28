#pragma once
#include <boost/bind/bind.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>  
#include <boost/asio/signal_set.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/filesystem/operations.hpp> // includes boost/filesystem/path.hpp
#include <boost/filesystem/fstream.hpp>
#include <boost/beast/ssl.hpp>
#include <iostream>
#include <cstdlib>
#include <functional>
#include <string>
#include <thread>
#include <cassert>
// void to silence unised warnings
#define assertm(exp, msg) assert(((void)msg, exp))
#include <chrono>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <memory>
#include <unordered_set>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;               // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using error_code = boost::system::error_code;

using string = std::string;
#define eprintf(...) fprintf(stderr, __VA_ARGS__)

// todo move some stuff to util
class SignalHook {
  net::signal_set signals;
public:
  SignalHook(net::io_context &ioc)
    : signals(ioc, SIGINT, SIGTERM) {
    signals.async_wait([&ioc](boost::system::error_code const&, int) {
      // Stop the io_context. This will cause run()
      // to return immediately, eventually destroying the
      // io_context and any remaining handlers in it.
      ioc.stop();
    });
  }
};

