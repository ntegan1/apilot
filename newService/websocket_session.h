#pragma once
#include "inc.h"
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
using namespace boost::interprocess;
// Forward declaration

/** Represents an active WebSocket connection to the server
*/
class websocket_session : public std::enable_shared_from_this<websocket_session>
{
    //net::io_context &ioc_;
    beast::flat_buffer buffer_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
    std::vector<std::shared_ptr<std::string const>> queue_;
    shared_memory_object shm_;

    void fail(error_code ec, char const* what);
    void on_accept(error_code ec);
    void on_read(error_code ec, std::size_t bytes_transferred);
    void on_write(error_code ec, std::size_t bytes_transferred);

public:
    websocket_session(
        beast::ssl_stream<beast::tcp_stream> &&stream);

    ~websocket_session();

    template<class Body, class Allocator>
    void
    run(http::request<Body, http::basic_fields<Allocator>> req);

    // Send a message
    void
    send(std::shared_ptr<std::string const> const& ss);
};

template<class Body, class Allocator>
void
websocket_session::
run(http::request<Body, http::basic_fields<Allocator>> req)
{
    // Accept the websocket handshake
    ws_.async_accept(
        req,
        std::bind(
            &websocket_session::on_accept,
            shared_from_this(),
            std::placeholders::_1));
}



// implementations

websocket_session::
websocket_session(
        beast::ssl_stream<beast::tcp_stream> &&stream)
    : ws_(std::move(stream))
    , shm_(create_only, "shm", read_write)
{
    shm_.truncate(1); // 1 byte
}

websocket_session::
~websocket_session()
{
    shared_memory_object::remove("shm");
    // Remove this session from the list of active sessions
    //state_->leave(*this);
}

void
websocket_session::
fail(error_code ec, char const* what)
{
    // Don't report these
    if( ec == net::error::operation_aborted ||
        ec == websocket::error::closed)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

void
websocket_session::
on_accept(error_code ec)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "accept");

    // Add this session to the list of active sessions
    //state_->join(*this);
    eprintf("[ws]: accept\n");

    // Read a message
    ws_.async_read(
        buffer_,
        [sp = shared_from_this()](
            error_code ec, std::size_t bytes)
        {
            sp->on_read(ec, bytes);
        });
}

void
websocket_session::
on_read(error_code ec, std::size_t size)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "read");

    // Send to all connections
    //state_->send(beast::buffers_to_string(buffer_.data()));
    //auto recv = beast::buffers_to_string(buffer_.data());
    if (size == 1) {
      mapped_region region(shm_, read_write);
      region.get_size();
      auto address = region.get_address();
      unsigned char a = 0;
      //boost::asio::buffer_size(buffer.data()));
      unsigned char const * ptr = boost::asio::buffer_cast<unsigned char const*>(beast::buffers_front(buffer_.data()));
      a = ptr[0];
      *(char *)address = a;
    } else if (size > 1) {
      //char a = *((char*) buffer_.data());
    } else { }
    //<< ", slider1: " << std::stoi(recv.c_str()) << std::endl;


    // Clear the buffer
    buffer_.consume(buffer_.size());

    // messaging
    //send(std::make_shared<std::string const>(std::move(Messaging(recv).response())));
    //send(std::make_shared<std::string const>(std::move(response)));
    //send(std::make_shared<std::string const>(std::move("")));

    // Read another message
    ws_.async_read(
        buffer_,
        [sp = shared_from_this()](
            error_code ec, std::size_t bytes)
        {
            sp->on_read(ec, bytes);
        });
}

void
websocket_session::
send(std::shared_ptr<std::string const> const& ss)
{
    // Always add to queue
    queue_.push_back(ss);

    // Are we already writing?
    if(queue_.size() > 1)
        return;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(*queue_.front()),
        [sp = shared_from_this()](
            error_code ec, std::size_t bytes)
        {
            sp->on_write(ec, bytes);
        });
}

void
websocket_session::
on_write(error_code ec, std::size_t)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "write");

    // Remove the string from the queue
    queue_.erase(queue_.begin());

    // Send the next message if any
    if(! queue_.empty())
        ws_.async_write(
            net::buffer(*queue_.front()),
            [sp = shared_from_this()](
                error_code ec, std::size_t bytes)
            {
                sp->on_write(ec, bytes);
            });
}
