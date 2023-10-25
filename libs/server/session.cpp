#include "session.hpp"

// public:
Session::Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket))
{
    ms_logger<DEBUG>::get_instance().log_to_stdout("Sesison::Session");
    ms_logger<INFO>::get_instance().log_to_file("Sesison::Session",DEBUG);
}
Session::~Session()
{
    ms_logger<DEBUG>::get_instance().log_to_stdout("Sesison::~Session");
    ms_logger<INFO>::get_instance().log_to_file("Sesison::~Session",DEBUG);
}

void Session::start()
{
    do_read();
}

// private:
void Session::do_read()
{
    auto self(shared_from_this());
    ms_logger<DEBUG>::get_instance().log_to_stdout("Sesison::do_read " + std::to_string(self.use_count()));
    ms_logger<INFO>::get_instance().log_to_file("Sesison::do_read " + std::to_string(self.use_count()),DEBUG);
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                                ms_logger<INFO>::get_instance().log_to_stdout("READ LENGTH " + std::to_string(length));
                                ms_logger<INFO>::get_instance().log_to_file("READ LENGTH " + std::to_string(length),INFO);

                                if (!ec)
                                {
                                    do_write(length);
                                }
                            });
}

void Session::do_write(std::size_t length)
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                                [this, self](boost::system::error_code ec, std::size_t /*length*/)
                                {
                                    if (!ec)
                                    {
                                        do_read();
                                    }
                                });
}
