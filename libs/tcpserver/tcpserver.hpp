#if !defined(SERVER_HPP)
#define SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <utility>
#include <functional>

#include <boost/asio.hpp>

#include "tcpsession.hpp"
#include "ms_logger.hpp"

template <class XmlParser>
class TCPServer
{
public:
TCPServer(
    boost::asio::io_context &io_context, 
    short port, 
    std::function<void(std::unordered_map<std::string,std::string>)> writeToDatabase
    )
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , xmlparser_(writeToDatabase)
{
    basic_log("TCPServer::TCPServer",DEBUG);
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
                // basic_log("making shared connection",DEBUG);
                std::make_shared<TCPSession<XmlParser>>(std::move(socket),xmlparser_)->start();
            }

            do_accept();
        });
}
boost::asio::ip::tcp::acceptor acceptor_;
XmlParser xmlparser_;

};
#endif // SERVER_HPP