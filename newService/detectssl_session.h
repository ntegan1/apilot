#pragma once
#include "inc.h"
#include "common.h"
#include "http_session.h"
#include "redirect_session.h"

// Detects SSL handshakes
class detect_session : public std::enable_shared_from_this<detect_session>
{
    //net::io_context& ioc_;
    beast::tcp_stream stream_;
    ssl::context& ctx_;
    beast::flat_buffer buffer_;
    string ip_{""};
    //unsigned short port_{0};

public:
    explicit
    detect_session(
        tcp::socket&& socket,
        ssl::context& ctx,
        string ip)
        : stream_(std::move(socket))
        , ctx_(ctx)
        , ip_(ip)

    {
    }

    // Launch the detector
    void
    run()
    {
        // Set the timeout.
       // stream_.expires_after(std::chrono::seconds(30));

        beast::async_detect_ssl(
            stream_,
            buffer_,
            beast::bind_front_handler(
                &detect_session::on_detect,
                this->shared_from_this()));
    }

    void
    on_detect(beast::error_code ec, boost::tribool result)
    {
        if(ec)
            return fail(ec, "detect");

        if(result)
        {
          std::printf("[ssl] session\n");
            // Launch SSL session
            beast::ssl_stream<beast::tcp_stream> stream(
              std::move(stream_), ctx_
            );
            std::make_shared<http_session>(
              std::move(stream),
              std::move(buffer_)
            )->run();
            return;
        }

        // https://www.boost.org/doc/libs/1_70_0/libs/beast/example/advanced/server-flex/advanced_server_flex.cpp
        // https://www.boost.org/doc/libs/1_80_0/doc/html/boost_asio/example/cpp03/ssl/server.cpp

        // Launch plain session
        // TODO run redirect http session to https
        //std::make_shared<http_session>(
        //  ioc_,
        //  std::move(stream_),
        //  std::move(buffer_)
        //)->run();
        std::make_shared<redirect_session>(
          std::move(stream_),
          std::move(buffer_),
          ip_
        )->run();
    }
};
