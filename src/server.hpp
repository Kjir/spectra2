#ifndef __SPECTRA2_SERVER_HPP_
#define __SPECTRA2_SERVER_HPP_
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <exception>
#include <sys/types.h>
#include <iostream>
#include <limits>
#include <sstream>
#define UDP_MAX_DGRAM 15000

#include "endian_fix.h"
#include "filter/source.hpp"
#include "debug.hpp"

class SequenceException : std::exception
{
    virtual const char *what() const throw()
    {
        return "Datagram out of sequence";
    }
};

template<class T>
class udp_sock : public SourceFilter
{
    public:
        udp_sock(std::string host, unsigned short port);
        size_t read_dgram(T *dgram);
        T *read(T *ret, size_t length);
        virtual void *read(void *ret, size_t length);
    private:
        boost::asio::io_service _io_service;
        boost::asio::ip::udp::socket _sock;
        boost::array<T,UDP_MAX_DGRAM> _buf;
        size_t _remaining;
        uint64_t _counter;

        void _move_to_front(T *arr, int start, int end = UDP_MAX_DGRAM);
};

template<class T> udp_sock<T>::udp_sock(std::string host = "localhost", unsigned short port = 50000) : _sock(_io_service), _remaining(0), _counter(0)
{
    using boost::asio::ip::udp;
    udp::resolver res(_io_service);
    udp::resolver::query query(udp::v4(), host, boost::lexical_cast<std::string>(port));
    udp::endpoint ep = *res.resolve(query);
    _sock.open(udp::v4());
    _sock.bind(ep);
    _counter = std::numeric_limits<uint64_t>::max();
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

    data_read -= sizeof(_counter);
    const uint64_t *long_ptr  = reinterpret_cast<uint64_t *>(buf.data());
    if( _counter == std::numeric_limits<uint64_t>::max() )
    {
        /*
         * Please note that this means there is no checking on the packet received
         * right after we reach the maximum length of uint64_t. This is the simplest
         * solution, although not the safest
         */
        //_counter = be64toh(*long_ptr) - 1;
        _counter = (*long_ptr) - 1;
    }
    //FIXME: What happens if long_ptr == 0?
    //if( _counter != be64toh(*long_ptr) - 1 )
    if( _counter != (*long_ptr) - 1 )
    {
        std::stringstream ss;
        ss << "Out of sequence: counter is " << _counter << " received sequence is "
            //<< be64toh(*long_ptr) << std::endl;
            << (*long_ptr) << std::endl;
        debug(ss.str());
        ss.clear(); ss.str("");
        ss << "Lost " << ((*long_ptr) - _counter) << " packets" << std::endl;
        notice(ss);
        _counter = std::numeric_limits<uint64_t>::max();
        throw SequenceException();
    }
    _counter++; long_ptr++;
    memcpy(dgram, long_ptr, data_read);
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
        try
        {
            /*
             * Read datagram
             */
            std::stringstream ss;
            /*
            ss << "Reading..." << std::endl;
            debug(ss.str());
            */
            size_t r = read_dgram( _buf.c_array() );
            //ss.clear(); ss.str("");
            //ss << "Read " << r << " bytes" << std::endl;
            //debug(ss.str());

            if (error && error != boost::asio::error::message_size)
            {
                throw boost::system::system_error(error);
            }

            if( r % sizeof(T) ) {
                //FIXME: Should not happen
                ss.clear();
                ss.str("");
                ss << "Not multiple of sizeof(T)" << std::endl;
                debug(ss.str());
            }
            r /= sizeof(T);

            /*
             * Append received datagram to tmp
             */
            for( int i = 0; i < r; i++ )
            {
                tmp.push_back(_buf[i]);
            }
        }
        catch(SequenceException se)
        {
            tmp.clear();
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

template<class T> void *udp_sock<T>::read(void *ret, size_t size)
{
    return read(reinterpret_cast<T *>(ret), size);
}

template<class T> void udp_sock<T>::_move_to_front(T *arr, int start, int length)
{
    for( int i = 0; i < length && start + i < UDP_MAX_DGRAM; i++)
    {
        _buf[i] = arr[start + i];
    }
}

#endif
