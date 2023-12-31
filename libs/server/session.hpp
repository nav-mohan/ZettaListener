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
    char data_[MAX_BUFFER];
    XmlParser& xmlparser_;

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
                                    xmlparser_.appendData(std::string(data_));
                                    do_read();
                                }
                                else 
                                {
                                    basic_log(std::string("ERROR READING " + ec.what()));
                                }
                            });
}

public:
Session(boost::asio::ip::tcp::socket socket, XmlParser& xp)
: socket_(std::move(socket))
, xmlparser_(xp)
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
