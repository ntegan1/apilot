#pragma once
#include "inc.h"
#include "detectssl_session.h"
#include "http_session.h"

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    ssl::context& ctx_;

    void fail(error_code ec, char const* what);
    void on_accept(error_code ec);

public:
    listener(
        net::io_context& ioc,
        ssl::context& ctx,
        tcp::endpoint endpoint);

    // Start accepting incoming connections
    void run();
};
listener::
listener(
    net::io_context& ioc,
    ssl::context& ctx,
    tcp::endpoint endpoint)
    : acceptor_(ioc)
    , socket_(ioc)
    , ctx_(ctx)
{
    error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if(ec)
    {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true));
    if(ec)
    {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if(ec)
    {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(
        net::socket_base::max_listen_connections, ec);
    if(ec)
    {
        fail(ec, "listen");
        return;
    }
}

void
listener::
run()
{
    // Start accepting a connection
    acceptor_.async_accept(
        socket_,
        [self = shared_from_this()](error_code ec)
        {

            self->on_accept(ec);
        });
}

// Report a failure
void
listener::
fail(error_code ec, char const* what)
{
    // Don't report on canceled operations
    if(ec == net::error::operation_aborted)
        return;
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handle a connection
void
listener::
on_accept(error_code ec)
{
    if(ec)
        return fail(ec, "accept");

    eprintf("\n[listener]\n");
    string ip = socket_.local_endpoint().address().to_string();
    //unsigned short port = socket_.local_endpoint().port();

    std::make_shared<detect_session>(
        std::move(socket_), ctx_, ip
        )->run();

    // Accept another connection
    acceptor_.async_accept(
        socket_,
        [self = shared_from_this()](error_code ec)
        {
            self->on_accept(ec);
        });
}

