#include "ms_logger.hpp"
#include "tcpserver.hpp"
#include "xmlparser.hpp"
#include "dbcon.hpp"
#include "webapi.hpp"
#include <functional>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("USAGE:   %s <TCP-PORT> <HTTP-PORT>\n",argv[0]);
        printf("EXAMPLE: %s 10001 8001\n",argv[0]);
        return 1;
    }
    int tcpPort = atoi(argv[1]);
    int httpPort = atoi(argv[2]);

    sqlite3 *db;
    int retval = sqlite3_open("zettalogger.db", &db);
    if (retval != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    DbCon dbc(db);

    boost::asio::io_context io_context;

    WebApi<DbCon> webapi(io_context, "0.0.0.0",httpPort, db); // i dont want the WebAPI making virtual calls
    
    auto writeToDatabase = [&dbc](std::unordered_map<std::string,std::string> record)
    {
        dbc.insertRecord(record);
        auto latestRecord = dbc.getLatest();
        // for(const auto [key,val] : latestRecord)
        // {
            // printf("LATEST %s --> %s\n",key.c_str(),val.c_str());
        // }
    };
    TCPServer<ZettaFullXmlParser> s(io_context, tcpPort, writeToDatabase);

    io_context.run();

    return 0;
}