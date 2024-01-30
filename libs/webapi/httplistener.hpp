#if !defined(LISTENER_HPP)
#define LISTENER_HPP

#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "httpsession.hpp"

// Accepts incoming connections and launches the sessions
template <class DbConnection>
class HTTPListener : public std::enable_shared_from_this<HTTPListener<DbConnection>>
{
private:
    boost::asio::io_context &ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    DbConnection &dbcon_;

public:
    HTTPListener(
        boost::asio::io_context &ioc,
        boost::asio::ip::tcp::endpoint endpoint,
        DbConnection &dbcon)
        : ioc_(ioc), acceptor_(boost::asio::make_strand(ioc)), dbcon_(dbcon)
    {
        boost::beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec)
        {
            fprintf(stderr, "ACCEPTOR FAILED TO OPEN %s\n", ec.what().c_str());
            return;
        }

        // Allow address reuse
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec)
        {
            fprintf(stderr, "ACCEPTOR FAILED TO SET_OPTION %s\n", ec.what().c_str());
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec)
        {
            fprintf(stderr, "ACCEPTOR FAILED TO BIND %s\n", ec.what().c_str());
            return;
        }

        // Start listening for connections
        acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec)
        {
            fprintf(stderr, "ACCEPTOR FAILED TO LISTEN %s\n", ec.what().c_str());
            return;
        }
        fprintf(stderr, "HTTPListener::HTTPListener\n");
    }

    // Start accepting incoming connections
    void run()
    {
        do_accept();
    }

    void do_accept()
    {
        fprintf(stderr, "HTTPListener::do_accept\n");
        acceptor_.async_accept(
            boost::asio::make_strand(ioc_),
            [&](boost::beast::error_code ec, boost::asio::ip::tcp::socket sock)
            {
                fprintf(stderr, "HTTPListener::async_accept\n");
                if (ec)
                {
                    fprintf(stderr, "ACCEPTOR FAILED TO ACCEP %s\n", ec.what().c_str());
                    return; // To avoid infinite loop
                }
                else
                {
                    // Create the session and run it
                    auto newSess = std::make_shared<HTTPSession<DbConnection>>(std::move(sock), dbcon_);
                    newSess->run();
                    fprintf(stderr, "MAKE SHARED SESSION\n");
                }

                // Accept another connection
                do_accept();
            });
    }
};

#endif // LISTENER_HPP
