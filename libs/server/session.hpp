#if !defined(SESION_HPP)
#define SESION_HPP
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

#include "ms_logger.hpp"

template<class XmlParser>
class Session : public std::enable_shared_from_this<Session<XmlParser>>
{
enum {MAX_BUFFER = 1024 };//keep this below size of 1 complete XML because REGEX cannot capture multiple XMLs
private:
    boost::asio::ip::tcp::socket socket_;
    std::vector<char> data_;
    XmlParser& xmlparser_;

void do_read()
{
    auto self(this->shared_from_this());
    basic_log("Sesison::do_read " + std::to_string(self.use_count()));
    socket_.async_receive(boost::asio::buffer(data_),
                            [this, self](boost::system::error_code ec, size_t length)
                            {
                                if (!ec)
                                {
                                    xmlparser_.appendData(data_,length);
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
, data_(MAX_BUFFER,0)
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
