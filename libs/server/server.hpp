#if !defined(SERVER_HPP)
#define SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <utility>

#include <boost/asio.hpp>

#include "session.hpp"
#include "ms_logger.hpp"

class Server
{
public:
    Server(boost::asio::io_context &io_context, short port);
private:
    void do_accept();
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif // SERVER_HPP