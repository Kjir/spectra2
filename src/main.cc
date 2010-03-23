#include <iostream>
#include <boost/threadpool.hpp>
#include <boost/bind.hpp>
#include <ipp.h>
#include "server.hpp"
#include "fft.hpp"

int main(int argc, char **argv)
{
    int order = 8;
    try {

        boost::threadpool::pool tp(5);
        int i = 1;
        Ipp16s *src, *dst;
        IppsFFTSpec_R_16s *spec = fft::allocSpec(&spec, order);

        dst = fft::alloc(dst, fft::order_to_length(order));
        fft::zero_mem(dst, fft::order_to_length(order));
        fft f(spec);
        udp_sock<Ipp16s> s("localhost", 50000);

        while(true)
        {
            src = s.read();

            tp.schedule(boost::bind(&fft::transform, &f, src, dst, order, 1, 12));
        }
    }
    catch( std::exception &e )
    {
        std::cerr << "Ex: " << e.what() << std::endl;
    }
}
