#include <iostream>
#include <list>
#include <boost/threadpool.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/circular_buffer.hpp>
#include <ipp.h>
#include "server.hpp"
#include "fft.hpp"
#include "fft_buf.hpp"

#define MAX_ITER 5

int main(int argc, char **argv)
{
    int order = 8;
    int num_threads = 5;
    int sums = 1;
    try {

        boost::threadpool::pool tp(num_threads);
        int i = 0;
        int siglen = fft::order_to_length(order);
        std::list<FFTBuf<Ipp16s> *> dst;
        boost::circular_buffer<SrcType<Ipp16s> > cbuf(num_threads * 3);

        ippStaticInit();
        IppsFFTSpec_R_16s *spec = fft::allocSpec(&spec, order);

        FFTBuf<Ipp16s> *b = new FFTBuf<Ipp16s>(siglen, sums);
        *b = fft::alloc(b->cdata(), siglen);
        fft::zero_mem(b->cdata(), siglen);
        dst.push_back(b);

        fft f(spec);
        udp_sock<Ipp16s> s("localhost", 50000);

        while(true)
        {
            std::cerr << "Reading: " << std::endl;
            SrcType<Ipp16s> src;
            src.data = fft::alloc(src.data, siglen);
            s.read(src.data, siglen); //try-catch for missed datagram
            if(cbuf.full() && !cbuf.front().erasable)
            {
                //FIXME: Do something to handle this situation
                std::cerr << "Circular buffer is too small!!!" << std::endl;
                return -9;
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

        std::cerr << "Waiting..." << std::endl;
        tp.wait();
        std::cerr << "...done!" << std::endl;

    }
    catch( std::exception &e )
    {
        std::cerr << "Ex: " << e.what() << std::endl;
    }

    return 0;
}
