#include <boost/asio.hpp>
#include <boost/array.hpp>

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

