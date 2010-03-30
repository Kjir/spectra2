#ifndef __SPECTRA2_FFT_BUF_HPP_
#define __SPECTRA2_FFT_BUF_HPP_
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

template<class T> class FFTBuf
{
    public:
        FFTBuf();
        FFTBuf(int siglen);
        FFTBuf(int siglen, int sums);
        T *get_data();
        void set_data(const T *buf);
        void set_siglen(int s) { _siglen = s; }
        void set_expected_sums(int s) { _expected_sums = s; }
        void lock();
        void unlock();
        void inc_processed();
        bool is_written();
        bool set_written();
        FFTBuf<T> & operator=(const T *rhs);
    private:
        T *_dst;
        std::queue<T *>src;
        int _siglen;
        int _processed_sums;
        int _expected_sums;
        bool _written;
        boost::mutex _mut;
        boost::unique_lock<boost::mutex> _lock;
};

template<class T> FFTBuf<T>::FFTBuf() : _siglen(0), _expected_sums(1), _processed_sums(0), _written(false), _lock(_mut)
{
}

template<class T> FFTBuf<T>::FFTBuf(int siglen) : _siglen(siglen), _expected_sums(1), _processed_sums(0), _written(false), _lock(_mut)
{
}

template<class T> FFTBuf<T>::FFTBuf(int siglen, int sums) : _siglen(siglen), _expected_sums(sums), _processed_sums(0), _written(false), _lock(_mut)
{
}

template<class T> T *FFTBuf<T>::get_data()
{
    return _dst;
}

template<class T> void FFTBuf<T>::set_data(const T *buf)
{
    lock();
    _dst = (T *)buf;
    unlock();
}

template<class T> void FFTBuf<T>::lock()
{
    if(!_lock.owns_lock())
    {
        _lock.lock();
    }
}

template<class T> void FFTBuf<T>::unlock()
{
    if(_lock.owns_lock()) {
        _lock.unlock();
    }
}

template<class T> void FFTBuf<T>::inc_processed()
{
    lock();
    _processed_sums++;
    unlock();
}

template<class T> bool FFTBuf<T>::is_written()
{
    return _written;
}

template<class T> bool FFTBuf<T>::set_written()
{
    if(_lock.try_lock())
    {
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

#endif
