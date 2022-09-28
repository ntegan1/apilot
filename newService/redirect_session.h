#pragma once
#include "inc.h"
#include "config.h"

/** Represents an established HTTP connection
*/
class redirect_session : public std::enable_shared_from_this<redirect_session>
{
    //net::io_context& ioc_;
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    string ip_;

    void fail(error_code ec, char const* what);
    void on_write(
        error_code ec, std::size_t, bool close);

public:
    // TODO better ip and port detection
    redirect_session(beast::tcp_stream&& stream, beast::flat_buffer&& buffer, string ip)
    : stream_(std::move(stream))
    , buffer_(std::move(buffer))
    , ip_(ip)
    {}

    void run();
};


void
redirect_session::
run()
{
    //string location = "https://" + ip_ + ":" + std::to_string(port_);
    //std::printf("location: %s\n", location.c_str());
    // Send the response
    auto const redirect_response =
    //[location]()
    []()
    {
        http::response<http::string_body> res{http::status::moved_permanently, 11};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::location, SERVER_SSL_REDIRECT_LOCATION);
        //res.set(http::field::location, location.c_str());
        //res.set(http::field::content_type, "text/html");
        //res.keep_alive(req.keep_alive());
        //res.body() = why.to_string();
        res.body() = "";
        res.prepare_payload();
        return res;
    };
  auto response = redirect_response();
  using response_type = typename std::decay<decltype(response)>::type;
  auto sp = std::make_shared<response_type>(std::forward<decltype(response)>(response));

  auto self = shared_from_this();
  http::async_write(this->stream_, *sp,
				[self, sp](
					error_code ec, std::size_t bytes)
				{
					self->on_write(ec, bytes, true); 
        }
	);

}

// Report a failure
void
redirect_session::
fail(error_code ec, char const* what)
{
    // Don't report on canceled operations
    if(ec == net::error::operation_aborted)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

void
redirect_session::
on_write(error_code ec, std::size_t, bool close)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "write");

    if(close)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        //socket_.shutdown(tcp::socket::shutdown_send, ec);
        stream_.close();
        return;
    }
}


