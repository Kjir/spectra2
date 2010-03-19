#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::udp;

int main(int argc, char **argv)
{
    try {
        boost::asio::io_service io_serv;
        udp::socket socket(io_serv, udp::endpoint(udp::v4(), 50000));

        while(true)
        {
            boost::array<char, 256> buf;
            udp::endpoint remote;
            boost::system::error_code error;
            socket.receive_from(boost::asio::buffer(buf), remote, 0, error);

            if (error && error != boost::asio::error::message_size)
            {
                throw boost::system::system_error(error);
            }
        }
    }
    catch( std::exception &e )
    {
        std::cerr << e.what() << std::endl;
    }
}
