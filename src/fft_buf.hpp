#ifndef __SPECTRA2_FFT_BUF_HPP_
#define __SPECTRA2_FFT_BUF_HPP_
#include <boost/thread/locks.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition_variable.hpp>

template<class T> class FFTBuf
{
    public:
        FFTBuf();
        FFTBuf(int siglen);
        FFTBuf(int siglen, int sums);
        T *cdata() { return _dst; }
        void set_data(const T *buf);
        void set_siglen(int s) { _siglen = s; }
        void set_expected_sums(int s) { _expected_sums = s; }
        void lock();
        void unlock();
        void wait();
        void notify_one() { _write_ready.notify_one(); }
        void notify_all() { _write_ready.notify_all(); }
        int inc_processed();
        int inc_assigned_sources() { return _assigned_sources++; }
        bool is_written();
        bool set_written();
        bool is_src_full() { return _assigned_sources == _expected_sums; }
        bool is_fully_processed() { return _processed_sums == _expected_sums; }
        FFTBuf<T> & operator=(const T *rhs);
    private:
        T *_dst;
        int _siglen;
        int _processed_sums;
        int _expected_sums;
        int _assigned_sources;
        bool _written;
        boost::recursive_mutex _mut;
        boost::condition_variable _write_ready;
};

template<class T> FFTBuf<T>::FFTBuf() : _dst(NULL), _siglen(0), _expected_sums(1), _processed_sums(0), _assigned_sources(0), _written(false)
{
}

template<class T> FFTBuf<T>::FFTBuf(int siglen) : _dst(NULL), _siglen(siglen), _expected_sums(1), _processed_sums(0), _assigned_sources(0), _written(false)
{
}

template<class T> FFTBuf<T>::FFTBuf(int siglen, int sums) : _dst(NULL), _siglen(siglen), _expected_sums(sums), _processed_sums(0), _assigned_sources(0), _written(false)
{
}

template<class T> void FFTBuf<T>::set_data(const T *buf)
{
    lock();

    _dst = (T *)buf;

    unlock();
}

template<class T> void FFTBuf<T>::lock()
{
    _mut.lock();
}

template<class T> void FFTBuf<T>::unlock()
{
    _mut.unlock();
}

template<class T> void FFTBuf<T>::wait()
{
    boost::unique_lock<boost::recursive_mutex> l(_mut);
    _write_ready.wait(l);
}

template<class T> int FFTBuf<T>::inc_processed()
{

    boost::unique_lock<boost::recursive_mutex> lock(_mut);

    _processed_sums++;

    if( is_fully_processed() )
    {
        notify_one();
    }

    return _processed_sums;
}

template<class T> bool FFTBuf<T>::is_written()
{
    boost::unique_lock<boost::recursive_mutex> lock(_mut);
    return _written;
}

template<class T> bool FFTBuf<T>::set_written()
{
    if(!_mut.try_lock())
    {
        return false;
    }
    if(_written)
    {
        unlock();
        return false;
    }

    _written = true;

    unlock();

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
        SrcType() : erasable(false), data(NULL) {}
        SrcType<T> & operator=(const SrcType<T> &rhs);
};

template<class T> SrcType<T> & SrcType<T>::operator=(const SrcType<T> &rhs)
{
    this->data = rhs.data;
    this->erasable = rhs.erasable;
    return *this;
}

#endif
