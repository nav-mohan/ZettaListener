#if !defined(SERVER_HPP)
#define SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <utility>

#include <boost/asio.hpp>

#include "session.hpp"
#include "ms_logger.hpp"

template <class XmlParser>
class Server
{
public:
Server(boost::asio::io_context &io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    basic_log("Server::Server",DEBUG);
    do_accept();
}

private:
void do_accept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                basic_log("making shared connection",DEBUG);
                std::make_shared<Session<XmlParser>>(std::move(socket))->start();
            }

            do_accept();
        });
}
boost::asio::ip::tcp::acceptor acceptor_;

};
#endif // SERVER_HPP