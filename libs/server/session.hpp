#if !defined(SESION_HPP)
#define SESION_HPP
#include <boost/asio.hpp>
#include <memory>
#include "ms_logger.hpp"

#define MAX_BUFFER_LEN 1024

template<class XmlParser>
class Session : public std::enable_shared_from_this<Session<XmlParser>>
{
private:
    boost::asio::ip::tcp::socket socket_;
    char data_[16];
    XmlParser xmlparser_;

void do_read()
{
    auto self(this->shared_from_this());
    basic_log("Sesison::do_read " + std::to_string(self.use_count()));
    socket_.async_read_some(boost::asio::buffer(data_, MAX_BUFFER_LEN),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                                basic_log("READ LENGTH " + std::to_string(length));

                                if (!ec)
                                {
                                    xmlparser_.parseXml(data_,length);
                                    do_write(length);
                                }
                            });
}

void do_write(std::size_t length)
{
    auto self(this->shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                                [this, self](boost::system::error_code ec, std::size_t len)
                                {
                                    if (!ec)
                                    {
                                        do_read();
                                    }
                                });
}

public:
Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket))
{
    basic_log("Sesison::Session",DEBUG);
}

~Session()
{
    basic_log("Sesison::~Session",DEBUG);
}

void start()
{
    do_read();
}


};
#endif // SESION_HPP
