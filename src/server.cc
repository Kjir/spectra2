#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/threadpool.hpp>
#include <boost/bind.hpp>
#include <ipp.h>

using boost::asio::ip::udp;

template<class T>
class udp_sock
{
    public:
        udp_sock(std::string host, unsigned short port);
        T *read();
    private:
        boost::asio::io_service _io_service;
        udp::socket _sock;
        boost::array<T,256> _buf;
};

template<class T> udp_sock<T>::udp_sock(std::string host = "localhost", unsigned short port = 50000) : _sock(_io_service)
{
    udp::resolver res(_io_service);
    udp::resolver::query query(udp::v4(), host, "50000");
    udp::endpoint ep = *res.resolve(query);
    ep.port(port);
    _sock.open(udp::v4());
    _sock.bind(ep);
}

template<class T> T *udp_sock<T>::read()
{
    udp::endpoint remote;
    boost::system::error_code error;
    _sock.receive_from(boost::asio::buffer(_buf), remote, 0, error);

    if (error && error != boost::asio::error::message_size)
    {
        throw boost::system::system_error(error);
    }

    return _buf.data();
}

void process(Ipp16s *data, int i) {
    std::cout << "Processing thread " << i << std::endl;
}

int main(int argc, char **argv)
{
    try {

        boost::threadpool::pool tp(5);
        udp_sock<Ipp16s> s("localhost", 50000);
        int i = 1;

        while(true)
        {
            Ipp16s *d;
            d = s.read();
            tp.schedule(boost::bind(process, d, i++));
        }
    }
    catch( std::exception &e )
    {
        std::cerr << "Ex: " << e.what() << std::endl;
    }
}
