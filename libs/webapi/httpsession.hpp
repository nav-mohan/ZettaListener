#if !defined(SESSION_HPP)
#define SESSION_HPP

#include <memory>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <regex>
#include "jsonutil.hpp"
#include "ms_logger.hpp"

// Handles an HTTP server connection
template <class DbConnection>
class HTTPSession : public std::enable_shared_from_this<HTTPSession<DbConnection>>
{
private:
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    DbConnection &dbcon_;

public:
    // Take ownership of the stream
    HTTPSession(
        boost::asio::ip::tcp::socket &&socket,
        DbConnection &dbcon)
        : stream_(std::move(socket)), dbcon_(dbcon)
    {
        // fprintf(stderr, "HTTPSession::HTTPSession\n");
        basic_log("HTTPSession CONSTRUCTED",TRACE);

    }

    ~HTTPSession()
    {
        basic_log("HTTPSession PURGED",TRACE);
        // fprintf(stderr, "HTTPSession::~HTTPSession\n");
    }

    // Start the asynchronous operation
    void run()
    {
        auto self(this->shared_from_this());
        // fprintf(stderr, "HTTPSession::run\n");
        boost::asio::dispatch(stream_.get_executor(), [self]()
                              { self->do_read(); });
    }

private:

    template <class Body, class Allocator>
    boost::beast::http::response<boost::beast::http::string_body> bad_request(
        const boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &req,
        boost::beast::string_view why
    )
    {
        basic_log("HTTPSession BAD-REQUEST",INFO);
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::bad_request, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;   
    }
    
    template <class Body, class Allocator>
    boost::beast::http::response<boost::beast::http::string_body> not_found(
        const boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &req,
        boost::beast::string_view target
    )
    {
        basic_log("HTTPSession NOT-FOUND",INFO);
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;   
    }
    
    template <class Body, class Allocator>
    boost::beast::http::response<boost::beast::http::string_body> send_ok(
        const boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &req,
        std::string resBody
    )
    {
        basic_log("HTTPSession REQUEST OK",INFO);
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "application/json");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        // printf("GET RQEUEST KEEP ALIVE %d\n",req.keep_alive());
        res.keep_alive(req.keep_alive());
        res.body() = std::move(resBody);
        res.prepare_payload();
        return res;   
    }

    template <class Body, class Allocator>
    boost::beast::http::response<boost::beast::http::string_body> server_error(
        const boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &req,
        boost::beast::string_view what
    )
    {
        basic_log("HTTPSession SERVER ERROR",INFO);
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::internal_server_error, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'.";
        res.prepare_payload();
        return res;   
    }

    template <class Body, class Allocator>
    boost::beast::http::message_generator handle_get(
        boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req)
    {
        std::string apiEndpoint = std::string(req.target());

        // Request path must match /latest/ or /date?YYYY-MM-DD
        if( std::regex_match(apiEndpoint, std::regex{"^/latest$"}))
        {
            std::unordered_map<std::string,std::string> latestRecord = std::move(dbcon_.getLatest());
            std::string jsonString = toJson(std::move(latestRecord));
            return send_ok(req,std::move(jsonString));
        }
        std::smatch matchRange;
        if( std::regex_match(apiEndpoint, matchRange, std::regex{"^/range\\?(\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z)\\&(\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z)$"}))
        {
            std::string startTime = "0000-00-00T00:00:00Z";
            std::string endTime = "0000-00-00T00:00:00Z";
            if(matchRange.size() > 2)
            {
                startTime = matchRange[1].str();
                endTime   = matchRange[2].str();
            }
            // printf("GETTING RANGE %s -- %s\n",startTime.c_str(),endTime.c_str());
            std::vector<std::unordered_map<std::string,std::string>> records = std::move(dbcon_.getByRange(startTime,endTime));
            std::string jsonString = toJson(std::move(records));
            return send_ok(req,std::move(jsonString));
        }
        std::smatch matchDate;
        if( std::regex_match(apiEndpoint, matchDate, std::regex{"^/date\\?(\\d{4}-\\d{2}-\\d{2})$"}))
        {
            std::string date = "0000-00-00";
            if(matchDate.size() > 1)    
                date = matchDate[1].str();
            // printf("GETTING DATE %s\n",date.c_str());
            std::vector<std::unordered_map<std::string,std::string>> records = std::move(dbcon_.getByDate(date));
            std::string jsonString = toJson(std::move(records));
            return send_ok(req,std::move(jsonString));
        }
        
        return server_error(req,"Unknown Server Error");
    }

    // Return a response for the given request.
    template <class Body, class Allocator>
    boost::beast::http::message_generator handle_request(
        boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req)
    {
        // Make sure we can handle the method
        switch (req.method())
        {
        case boost::beast::http::verb::get:
            // printf("GET REQUEST %s\n",std::string(req.target()).c_str());
            return handle_get(std::move(req));

        case boost::beast::http::verb::put:
            // printf("PUT REQUEST\n");
            break;

        case boost::beast::http::verb::delete_:
            // printf("DELETE REQUEST\n");
            break;

        case boost::beast::http::verb::post:
            // printf("POST REQUEST\n");
            break;

        default:
            break;
        }
        return bad_request(req, "Unknown HTTP verb");
    }

    void do_read()
    {
        // fprintf(stderr, "HTTPSession::do_read\n");

        stream_.expires_after(std::chrono::seconds(30));

        auto self(this->shared_from_this());
        self->req_ = {}; // Make the request empty before reading, otherwise the operation behavior is undefined.

        boost::beast::http::async_read(
            self->stream_, self->buffer_, self->req_, [self](boost::beast::error_code ec, std::size_t len)
            {
            // fprintf(stderr,"HTTPSession::async_read\n");
            boost::ignore_unused(len);

            // This means they closed the connection
            if (ec == boost::beast::http::error::end_of_stream)
                return self->do_close();

            if (ec)
            {
                // fprintf(stderr, "FAILED TO READ: %s\n", ec.what().c_str());
                basic_log("FAILED TO READ: " + ec.what());
                return;
            }

            // Send the response
            self->send_response(
                    self->handle_request(std::move(self->req_))
                ); }

        );
    }

    void send_response(boost::beast::http::message_generator &&msg)
    {
        bool keep_alive = msg.keep_alive();
        // fprintf(stderr, "SEND RESPONSE\n");

        auto self(this->shared_from_this());
        boost::beast::async_write(
            self->stream_,
            std::move(msg),
            [self, &keep_alive](boost::beast::error_code ec, std::size_t len)
            {
                boost::ignore_unused(len);
                if (ec)
                {
                    // fprintf(stderr, "FAILED TO WRITE: %s\n", ec.what().c_str());
                    basic_log("FAILED TO WRITE: " + ec.what());
                    return;
                }

                if (!keep_alive)
                {
                    // fprintf(stderr, "CLOSING CONNECTION\n");
                    basic_log("CLOSING CONNECTION...");
                    return self->do_close();
                }

                // fprintf(stderr, "DO ANOTHE READ\n");
                self->do_read();
            });
    }

    void do_close()
    {
        basic_log("HTTPSession CLOSING SESSION...",INFO);
        // fprintf(stderr, "DO CLOSE\n");
        // Send a TCP shutdown
        boost::beast::error_code ec;
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        // At this point the connection is closed gracefully
    }
};

#endif // SESSION_HPP
