#include "fft.hpp"
#include <iostream>
#include <stdlib.h>
#include <boost/numeric/conversion/cast.hpp>
#include <math.h>

fft::fft(const IppsFFTSpec_R_16s *spec) : FFTSpec(spec), buffer(NULL) {
    IppStatus status;
    int bufsize;

    status = ippsFFTGetBufSize_R_16s( FFTSpec, &bufsize );
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in FFTGetBufSize: " << ippGetStatusString(status) << "\n";
        exit(2);
    }
    buffer = ippsMalloc_8u(bufsize); //FIXME: Check if buffer is thread safe
    if( buffer == NULL ) {
        std::cerr << "Not enough memory\n";
        exit(3);
    }
}

IppsFFTSpec_R_16s *fft::allocSpec(IppsFFTSpec_R_16s **spec, int order, bool fast) {
    IppStatus status;
    IppHintAlgorithm hint;

    /*
     * Choose the algorithm type: fast or accurate
     */
    if(fast) {
        hint = ippAlgHintFast;
    } else {
        hint = ippAlgHintAccurate;
    }

    status = ippsFFTInitAlloc_R_16s(spec, order, IPP_FFT_NODIV_BY_ANY, hint);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        exit(1);
    }

    return *spec;
}

Ipp16s *fft::transform(SrcType<Ipp16s> &src, FFTBuf<Ipp16s> & data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp16s *tmpdst;
    int siglen = fft::order_to_length(order);
    
    tmpdst = fft::alloc(data.cdata(), siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src.data, tmpdst, FFTSpec, scaling, buffer);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        exit(4);
    }
    Ipp16sc *vc, zero = {0, 0};
    vc = fft::alloc(vc, siglen);
    if( vc == NULL ) {
        std::cerr << "Not enough memory\n";
        exit(3);
    }
    //Set the vector to zero
    ippsSet_16sc(zero, vc, siglen);
    status = ippsConjPack_16sc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        exit(5);
    }
    status = ippsPowerSpectr_16sc_Sfs(vc, tmpdst, siglen, pscaling);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        exit(6);
    }
    fft::free(vc);
    vc = NULL;

    data.lock();
    status = ippsAdd_16s_I(tmpdst, data.cdata(), siglen);
    data.unlock();
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(src.data);
    src.erasable = true;
    data.lock();
    data.inc_processed();
    if(data.is_fully_processed())
    {
        data.write_ready.notify_one();
    }
    data.unlock();
    return data.cdata();
}

int fft::order_to_length(int order) {
    return boost::numeric_cast<int>( pow(2, order) );
}

Ipp16s *fft::alloc(Ipp16s *d, int length) {
    d = ippsMalloc_16s(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

Ipp16sc *fft::alloc(Ipp16sc *d, int length) {
    d = ippsMalloc_16sc(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

void fft::zero_mem(Ipp16s *d, int length) {
    ippsZero_16s(d, length);
}
