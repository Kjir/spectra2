#include <iostream>
#include <boost/threadpool.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <ipp.h>
#include "server.hpp"
#include "fft.hpp"

#define MAX_ITER 5

int main(int argc, char **argv)
{
    int order = 8;
    try {

        boost::threadpool::pool tp(5);
        int i = 0;
        int siglen = fft::order_to_length(order);
        boost::array<Ipp16s *, MAX_ITER> src, dst;
        IppsFFTSpec_R_16s *spec = fft::allocSpec(&spec, order);

        for( int k = 0; k < MAX_ITER; k++) {
            dst[k] = fft::alloc(dst[k], siglen);
            fft::zero_mem(dst[k], siglen);
        }
        fft f(spec);
        udp_sock<Ipp16s> s("localhost", 50000);

        for(i = 0; i < MAX_ITER; i++)
        {
            std::cerr << "Reading: " << std::endl;
            s.read(src[i], siglen);

            tp.schedule(boost::bind(&fft::transform, &f, src[i], dst[i], order, 1, 12));
        }

        std::cerr << "Waiting..." << std::endl;
        tp.wait();
        std::cerr << "...done!" << std::endl;

        for(i=0; i < MAX_ITER; i++)
        {
            std::cout.write((char *)src[i], sizeof(src) * siglen);
            std::cout << "break" << std::endl;
        }
        std::cout.flush();
    }
    catch( std::exception &e )
    {
        std::cerr << "Ex: " << e.what() << std::endl;
    }
}
