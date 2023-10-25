#if !defined(SESION_HPP)
#define SESION_HPP

#include <boost/asio.hpp>
#include <memory>

#include "ms_logger.hpp"

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::ip::tcp::socket socket);
    ~Session();
    void start();

private:
    void do_read();
    void do_write(std::size_t length);
    boost::asio::ip::tcp::socket socket_;
    enum
    {
        max_length = 16
    };
    char data_[max_length];
};

#endif // SESION_HPP
