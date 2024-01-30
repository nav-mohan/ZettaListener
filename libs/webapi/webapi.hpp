#if !defined(WEBAPI_HPP)
#define WEBAPI_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sqlite3.h>

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "httplistener.hpp"

template <class DbConnection> // i dont want the webapi making virtual calls
class WebApi
{
private:
    boost::asio::io_context& ioc_;
    DbConnection dbcon_;
    std::shared_ptr<HTTPListener<DbConnection>> listener_;

public:
    WebApi(boost::asio::io_context& ioc, const std::string addr, const short unsigned int port, sqlite3 *db) 
    : ioc_(ioc), dbcon_(db)
    {
        auto const address = boost::asio::ip::make_address(addr.c_str());
        // Create and launch a listening port
        listener_ = std::make_shared<HTTPListener<DbConnection>>(
            ioc_,
            boost::asio::ip::tcp::endpoint{address, port},
            dbcon_
            );
        listener_->run();
    }
};

#endif // WEBAPI_HPP
