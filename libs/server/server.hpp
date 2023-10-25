#if !defined(SERVER_HPP)
#define SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <utility>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#include "session.hpp"

class Server
{
public:
    Server(boost::asio::io_context &io_context, short port);
private:
    void do_accept();
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif // SERVER_HPP