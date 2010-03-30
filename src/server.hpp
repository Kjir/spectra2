#ifndef __SPECTRA2_SERVER_HPP_
#define __SPECTRA2_SERVER_HPP_
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <iostream>
#define UDP_MAX_DGRAM 15000

template<class T>
class udp_sock
{
    public:
        udp_sock(std::string host, unsigned short port);
        size_t read_dgram(T *dgram);
        T *read(T *ret, size_t length);
    private:
        boost::asio::io_service _io_service;
        boost::asio::ip::udp::socket _sock;
        boost::array<T,UDP_MAX_DGRAM> _buf;
        size_t _remaining;

        void _move_to_front(T *arr, int start, int end = UDP_MAX_DGRAM);
};

template<class T> udp_sock<T>::udp_sock(std::string host = "localhost", unsigned short port = 50000) : _sock(_io_service), _remaining(0)
{
    using boost::asio::ip::udp;
    udp::resolver res(_io_service);
    udp::resolver::query query(udp::v4(), host, boost::lexical_cast<std::string>(port));
    udp::endpoint ep = *res.resolve(query);
    _sock.open(udp::v4());
    _sock.bind(ep);
}

template<class T> size_t udp_sock<T>::read_dgram(T *dgram)
{
    using boost::asio::ip::udp;
    udp::endpoint remote;
    boost::system::error_code error;
    boost::array<T,UDP_MAX_DGRAM> buf;
    size_t data_read = _sock.receive_from(boost::asio::buffer(buf), remote, 0, error);

    if (error && error != boost::asio::error::message_size)
    {
        throw boost::system::system_error(error);
    }

    memcpy(dgram, buf.data(), data_read);
    return data_read;
}

template<class T> T *udp_sock<T>::read(T *ret, size_t size)
{
    using boost::asio::ip::udp;
    udp::endpoint remote;
    boost::system::error_code error;
    std::vector<T> tmp;
    size_t received = 0;
    int i = 0;

    tmp.reserve(size);

    /*
     * Fill in tmp what we had left over from last read
     */
    for( int i = 0; i < _remaining; i++ )
    {
        tmp.push_back(_buf[i]);
    }

    /*
     * Loop until we have at least size in our tmp buffer
     */
    while(tmp.size() < size)
    {
        /*
         * Read datagram
         */
        std::cerr << "Reading from socket" << std::endl;
        size_t r = _sock.receive_from(boost::asio::buffer(_buf), remote, 0, error);

        /*
         * Append received datagram to tmp
         */
        for( int i = 0; i < r; i++ )
        {
            tmp.push_back(_buf[i]);
        }

        if (error && error != boost::asio::error::message_size)
        {
            throw boost::system::system_error(error);
        }
    }

    /*
     * Copy size elements to ret
     */
    for(int i = 0; i < size; i++)
    {
        ret[i] = tmp[i];
    }
    _remaining = tmp.size() - size;
    /*
     * Move the remaining data in tmp to the front of _buf
     */
    _move_to_front(tmp.data(), size, _remaining);

    return ret;
}

template<class T> void udp_sock<T>::_move_to_front(T *arr, int start, int length)
{
    for( int i = 0; start < start + length && i < UDP_MAX_DGRAM && start < UDP_MAX_DGRAM; i++, start++)
    {
        _buf[i] = arr[start];
    }
}

#endif
