#include "server.hpp"

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            printf("USAGE: %s <PORT>\n",argv[0]);
            return 1;
        }

        boost::asio::io_context io_context;

        Server s(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}