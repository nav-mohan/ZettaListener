#include "server.hpp"

// class Server
// {
// public:
    Server::Server(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        ms_logger<INFO>::get_instance().log_to_stdout("Server::Server");
        ms_logger<INFO>::get_instance().log_to_file("Server::Server",DEBUG);
        do_accept();
    }

// private:
    void Server::do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    ms_logger<INFO>::get_instance().log_to_stdout("making shared connection");
                    ms_logger<INFO>::get_instance().log_to_file("making shared connection",DEBUG);
                    std::make_shared<Session>(std::move(socket))->start();
                }

                do_accept();
            });
    }
// }; class Server