#ifndef __SPECTRA2_FFT_BUF_HPP_
#define __SPECTRA2_FFT_BUF_HPP_
/**
 * @file
 * @author Stéphane Bisinger <stephane.bisinger@gmail.com>
 *
 * @section DESCRIPTION
 *
 * Classes used ad input and output buffers
 */
#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <sstream>
#include "debug.hpp"
#include "ipp.hpp"

/**
 * @brief Output buffer for an FFT
 *
 * This class represents a buffer which contains the output vector for an FFT.
 * This buffer is assigned to different FFTs for integration purposes, meaning
 * that the result of the computation is added to the output vector.
 */
template<class T> class FFTBuf
{
    public:
        /**
         * Constructor
         *
         * @param siglen The length of the signal
         */
        FFTBuf(long int siglen);
        /**
         * Constructor
         *
         * @param siglen The length of the signal
         * @param sums The number of integrations expected before processing is
         * finished
         */
        FFTBuf(long int siglen, int sums);
        ///Destructor
        ~FFTBuf();
        /**
         * Return the direct pointer to the data
         * @return Pointer to the data member
         */
        T *cdata() { return _dst; }
        /**
         * Update the internal pointer
         *
         * @param buf The new pointer to the data
         */
        void set_data(const T *buf);
        /**
         * Update the length of the signal
         *
         * @param s The new length of the signal
         */
        void set_siglen(int s) { _siglen = s; }
        /**
         * Retrieve the signal length
         *
         * @return The length of the signal
         */
        int get_siglen() { return _siglen; }
        /**
         * Set the number of expected integrations
         *
         * @param s The number of expected integrations
         */
        void set_expected_sums(int s) { _expected_sums = s; }
        /**
         * Get a reference to the mutex
         *
         * @return A reference to the mutex
         */
        boost::mutex &get_mutex();
        /**
         * Halt execution until this buffer is ready to be written to output
         */
        void wait_until_processed();
        void notify_one() { _write_ready.notify_one(); }
        void notify_all() { _write_ready.notify_all(); }
        int inc_processed();
        int inc_assigned_sources();
        bool is_written();
        bool set_written();
        bool is_src_full() { return _assigned_sources == _expected_sums; }
        FFTBuf<T> & operator=(const T *rhs);
    private:
        bool _is_fully_processed() { return _processed_sums == _expected_sums; }
    private:
        T *_dst;
        long int _siglen;
        int _processed_sums;
        int _expected_sums;
        int _assigned_sources;
        bool _written;
        boost::mutex _mut;
        boost::condition_variable _write_ready;
        FFTBuf();
};

template<class T> FFTBuf<T>::FFTBuf() : _dst(NULL), _siglen(0), _expected_sums(1), _processed_sums(0), _assigned_sources(0), _written(false)
{
    std::stringstream ss;
    ss << "This is " << std::hex << this << std::dec << std::endl;
    debug(ss.str());
}

template<class T> FFTBuf<T>::FFTBuf(long int siglen) : _dst(NULL), _siglen(siglen), _expected_sums(1), _processed_sums(0), _assigned_sources(0), _written(false)
{
    std::stringstream ss;
    ss << "This is " << std::hex << this << std::dec << std::endl;
    debug(ss.str());
    _dst = IPP::alloc(_dst, siglen);
    IPP::zero_mem(_dst, siglen);
}

template<class T> FFTBuf<T>::FFTBuf(long int siglen, int sums) : _dst(NULL), _siglen(siglen), _expected_sums(sums), _processed_sums(0), _assigned_sources(0), _written(false)
{
    std::stringstream ss;
    ss << "This is " << std::hex << this << std::dec << std::endl;
    debug(ss.str());
    _dst = IPP::alloc(_dst, siglen);
    IPP::zero_mem(_dst, siglen);
}

template<class T> FFTBuf<T>::~FFTBuf()
{
    std::stringstream ss;
    ss << "Deconstructing FFT Buffer: " << std::hex << this << std::dec << std::endl;
    debug(ss.str());
    notify_all();
    IPP::free(_dst);
    _dst = NULL;
}

template<class T> void FFTBuf<T>::set_data(const T *buf)
{
    boost::unique_lock<boost::mutex> lock(_mut);
    _dst = (T *)buf;
}

template<class T> boost::mutex & FFTBuf<T>::get_mutex()
{
    return _mut;
}

template<class T> void FFTBuf<T>::wait_until_processed()
{
    boost::unique_lock<boost::mutex> l(_mut);
    if(!_is_fully_processed()) {
        std::stringstream ss;
        ss << "Waiting while processing: " << std::hex << &_mut << std::dec << std::endl;
        debug(ss.str());
        _write_ready.wait(l);
        ss.clear(); ss.str("");
        ss << "Processed " << std::hex << this << std::dec;
        ss << "! Mutex " << std::hex << &_mut << std::dec << " is free" << std::endl;
        debug(ss.str());
    }
}

template<class T> int FFTBuf<T>::inc_assigned_sources()
{ 
    if(this == NULL) {
        std::stringstream ss;
        ss << "Class no longer exists" << std::endl;
        debug(ss.str());
    }
    //boost::unique_lock<boost::mutex> lock(_mut);
    _assigned_sources += 1;
    return _assigned_sources;
}

template<class T> int FFTBuf<T>::inc_processed()
{

    boost::unique_lock<boost::mutex> lock(_mut);

    _processed_sums++;

    std::stringstream ss;
    ss << "assigned sources: " << _assigned_sources << std::endl;
    ss << "expected sums: " << _expected_sums << std::endl;
    debug(ss.str());
    if( _is_fully_processed() )
    {
        notify_one();
    }

    return _processed_sums;
}

template<class T> bool FFTBuf<T>::is_written()
{
    boost::unique_lock<boost::mutex> lock(_mut);
    return _written;
}

template<class T> bool FFTBuf<T>::set_written()
{
    boost::unique_lock<boost::mutex> lock(_mut, boost::try_to_lock_t());
    if(!lock.owns_lock())
    {
        return false;
    }
    if(_written)
    {
        return false;
    }

    _written = true;

    return true;
} 

template<class T> FFTBuf<T> & FFTBuf<T>::operator=(const T *rhs)
{
    set_data(rhs);
    return *this;
}

template<class T> struct SrcType {
    public:
        T *data;
        mutable bool erasable;
        mutable boost::mutex *mutex;
        SrcType();
        SrcType(const SrcType &other);
        SrcType<T> & operator=(const SrcType<T> &rhs);
        ~SrcType();
};

template<class T> SrcType<T>::SrcType() : erasable(false), data(NULL), mutex(NULL)
{
    this->mutex = new boost::mutex;
}

template<class T> SrcType<T>::SrcType(const SrcType &other) : erasable(other.erasable), data(other.data), mutex(NULL)
{
    this->mutex = new boost::mutex;
}

template<class T> SrcType<T>::~SrcType()
{
    delete mutex;
}
template<class T> SrcType<T> & SrcType<T>::operator=(const SrcType<T> &rhs)
{
    this->data = rhs.data;
    this->erasable = rhs.erasable;
    return *this;
}

#endif
