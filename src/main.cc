#include <iostream>
#include <list>
#include <boost/threadpool.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/circular_buffer.hpp>
#include <ipp.h>
#include "server.hpp"
#include "fft.hpp"
#include "fft_buf.hpp"

template<class T> class List {
    private:
        std::list<T> _dst;
        boost::mutex _mut;
        boost::condition_variable _empty;
    public:
        void pop_front() { _dst.pop_front(); }
        void push_back(const T &x);
        T &front() { return _dst.front(); }
        T &back();
        void wait();
        void notify_one() { _empty.notify_one(); }
        void notify_all() { _empty.notify_all(); }
        bool empty();
};

template<class T> bool List<T>::empty() {
    boost::mutex::scoped_lock lock(_mut);
    return _dst.empty();
}

template<class T> void List<T>::push_back(const T &x) {
    boost::mutex::scoped_lock lock(_mut);
    _dst.push_back(x);
    notify_one();
}

template<class T> T& List<T>::back() {
    boost::mutex::scoped_lock lock(_mut);
    return _dst.back();
}

template<class T> void List<T>::wait() {
    boost::mutex::scoped_lock lock(_mut);
    return _empty.wait(lock);
}

void output(List<FFTBuf<Ipp16s> *> &l, std::ostream &s)
{
    while(true)
    {
        while(l.empty())
        {
            std::cerr << "Before empty wait" << std::endl;
            l.wait();
            std::cerr << "After empty wait" << std::endl;
        }
        FFTBuf<Ipp16s> *f = l.front();

        if(f->is_written()) {
            delete f;
            l.pop_front();
            continue;
        }

        while( !f->is_fully_processed() )
        {
            std::cerr << "Before wait" << std::endl;
            f->wait();
            std::cerr << "After wait" << std::endl;
        }
        if(!f->set_written())
        {
            continue;
        }
        {
            boost::mutex::scoped_lock lock(f->get_mutex());
            s.write((char *)f->cdata(), sizeof(*(f->cdata())) * f->get_siglen());
        }
        delete f;
        l.pop_front();
    }
    std::cerr << "Quit" << std::endl;
}

int main(int argc, char **argv)
{
    int order = 8;
    int num_threads = 5;
    int sums = 1;
    ippStaticInit();
    try {

        boost::threadpool::pool tp(num_threads);
        int i = 0;
        int siglen = fft::order_to_length(order);
        List<FFTBuf<Ipp16s> *> dst;
        boost::circular_buffer<SrcType<Ipp16s> > cbuf(num_threads * 3);

        IppsFFTSpec_R_16s *spec = fft::allocSpec(&spec, order);

        FFTBuf<Ipp16s> *b = new FFTBuf<Ipp16s>(siglen, sums);
        *b = fft::alloc(b->cdata(), siglen);
        fft::zero_mem(b->cdata(), siglen);
        dst.push_back(b);

        fft f(spec); //The FFT object
        udp_sock<Ipp16s> s("localhost", 50000); //The UDP server
        boost::thread out_thread(output, boost::ref(dst), boost::ref(std::cout));

        while(true)
        {
            SrcType<Ipp16s> src;
            src.data = fft::alloc(src.data, siglen);
            s.read(src.data, siglen); //try-catch for missed datagram
            if(!cbuf.empty())
            {
                boost::mutex::scoped_lock lock(*cbuf.front().mutex);
                while(cbuf.full() && !cbuf.front().erasable)
                {
                    lock.unlock();
                    tp.wait(num_threads/2);
                    lock.lock();
                    std::cerr << "Circular buffer is too small!!!" << std::endl;
                }
            }
            cbuf.push_back(src);

            if( dst.empty() || dst.back()->is_src_full() ) {
                b = new FFTBuf<Ipp16s>(siglen, sums);
                *b = fft::alloc(b->cdata(), siglen);
                fft::zero_mem(b->cdata(), siglen);
                dst.push_back(b);
            }
            tp.schedule(boost::bind(&fft::transform, &f, boost::ref(cbuf.back()), boost::ref(*(dst.back())), order, 1, 12));
            dst.back()->inc_assigned_sources();
        }

    }
    catch( std::exception &e )
    {
        std::cerr << "Ex: " << e.what() << std::endl;
    }

    return 0;
}
