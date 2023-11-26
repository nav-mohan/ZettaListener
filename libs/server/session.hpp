#if !defined(SESION_HPP)
#define SESION_HPP
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include "ms_logger.hpp"

template<class XmlParser>
class Session : public std::enable_shared_from_this<Session<XmlParser>>
{
enum {MAX_BUFFER = 4096};
private:
    boost::asio::ip::tcp::socket socket_;
    XmlParser xmlparser_;
    char data_[MAX_BUFFER];

void do_read()
{
    memset(data_,0,MAX_BUFFER);
    auto self(this->shared_from_this());
    basic_log("Sesison::do_read " + std::to_string(self.use_count()));
    socket_.async_read_some(boost::asio::buffer(data_,MAX_BUFFER),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                                if (!ec)
                                {
                                    xmlparser_.appendData(std::move(data_));
                                    // do_write(length);
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
