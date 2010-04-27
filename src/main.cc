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
#include "list.hpp"
#include "output.hpp"

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
                FFTBuf<Ipp16s> *b = new FFTBuf<Ipp16s>(siglen, sums);
                *b = fft::alloc(b->cdata(), siglen);
                fft::zero_mem(b->cdata(), siglen);
                dst.push_back(b);
            }
            tp.schedule(
                    boost::bind(
                        static_cast<Ipp16s *(fft::*) (const SrcType<Ipp16s> &, FFTBuf<Ipp16s> &, int, int, int)>(&fft::transform), &f, boost::cref(cbuf.back()), boost::ref(*(dst.back())), order, 1, 12
                        )
                    );
            dst.back()->inc_assigned_sources();
        }

    }
    catch( std::exception &e )
    {
        std::cerr << "Ex: " << e.what() << std::endl;
    }

    return 0;
}
