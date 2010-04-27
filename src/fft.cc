#include "fft.hpp"
#include <iostream>
#include <stdlib.h>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/thread/mutex.hpp>
#include <math.h>

/* Constructors */

fft::fft(const IppsFFTSpec_R_16s *spec) : R_16s(spec), buffer(NULL) {
    /*
    IppStatus status;
    int bufsize;

    status = ippsFFTGetBufSize_R_16s( FFTSpec, &bufsize );
    if( status != fftStsNoErr ) {
        std::cerr << "IPP Error in FFTGetBufSize: " << ippGetStatusString(status) << "\n";
        exit(2);
    }
    buffer = ippsMalloc_8u(bufsize); //TODO: Create some buffer handling thingy
    if( buffer == NULL ) {
        std::cerr << "Not enough memory\n";
        exit(3);
    }
    */
}

fft::fft(const IppsFFTSpec_R_32s *spec) : R_32s(spec), buffer(NULL) {}
fft::fft(const IppsFFTSpec_R_32f *spec) : R_32f(spec), buffer(NULL) {}
fft::fft(const IppsFFTSpec_R_64f *spec) : R_64f(spec), buffer(NULL) {}

/* AllocSpec */

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

IppsFFTSpec_R_32s *fft::allocSpec(IppsFFTSpec_R_32s **spec, int order, bool fast) {
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

    status = ippsFFTInitAlloc_R_32s(spec, order, IPP_FFT_NODIV_BY_ANY, hint);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        exit(1);
    }

    return *spec;
}

IppsFFTSpec_R_32f *fft::allocSpec(IppsFFTSpec_R_32f **spec, int order, bool fast) {
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

    status = ippsFFTInitAlloc_R_32f(spec, order, IPP_FFT_NODIV_BY_ANY, hint);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        exit(1);
    }

    return *spec;
}

IppsFFTSpec_R_64f *fft::allocSpec(IppsFFTSpec_R_64f **spec, int order, bool fast) {
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

    status = ippsFFTInitAlloc_R_64f(spec, order, IPP_FFT_NODIV_BY_ANY, hint);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        exit(1);
    }

    return *spec;
}

/* Transform */

Ipp16s *fft::transform(const SrcType<Ipp16s> &src, FFTBuf<Ipp16s> & data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp16s *tmpdst;
    int siglen = fft::order_to_length(order);
    
    tmpdst = fft::alloc(data.cdata(), siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src.data, tmpdst, R_16s, scaling, buffer);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        exit(4);
    }
    Ipp16sc *vc;
    vc = fft::alloc(vc, siglen);
    if( vc == NULL ) {
        std::cerr << "Not enough memory\n";
        exit(3);
    }
    //Set the vector to zero
    fft::zero_mem(vc, siglen);
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

    {
        boost::mutex::scoped_lock lock(data.get_mutex());
        status = ippsAdd_16s_I(tmpdst, data.cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    data.inc_processed();
    return data.cdata();
}

Ipp32f *fft::transform(const SrcType<Ipp32f> &src, FFTBuf<Ipp32f> & data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp32f *tmpdst;
    int siglen = fft::order_to_length(order);

    tmpdst = fft::alloc(data.cdata(), siglen);

    status = ippsFFTFwd_RToPack_32f(src.data, tmpdst, R_32f, buffer);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        exit(4);
    }
    Ipp32fc *vc;
    vc = fft::alloc(vc, siglen);
    if( vc == NULL ) {
        std::cerr << "Not enough memory\n";
        exit(3);
    }
    //Set the vector to zero
    fft::zero_mem(vc, siglen);
    status = ippsConjPack_32fc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        exit(5);
    }
    status = ippsPowerSpectr_32fc(vc, tmpdst, siglen);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        exit(6);
    }
    fft::free(vc);
    vc = NULL;

    {
        boost::mutex::scoped_lock lock(data.get_mutex());
        status = ippsAdd_32f_I(tmpdst, data.cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    data.inc_processed();
    return data.cdata();
}

Ipp64f *fft::transform(const SrcType<Ipp64f> &src, FFTBuf<Ipp64f> & data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp64f *tmpdst;
    int siglen = fft::order_to_length(order);

    tmpdst = fft::alloc(data.cdata(), siglen);

    status = ippsFFTFwd_RToPack_64f(src.data, tmpdst, R_64f, buffer);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        exit(4);
    }
    Ipp64fc *vc;
    vc = fft::alloc(vc, siglen);
    if( vc == NULL ) {
        std::cerr << "Not enough memory\n";
        exit(3);
    }
    //Set the vector to zero
    fft::zero_mem(vc, siglen);
    status = ippsConjPack_64fc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        exit(5);
    }
    status = ippsPowerSpectr_64fc(vc, tmpdst, siglen);
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        exit(6);
    }
    fft::free(vc);
    vc = NULL;

    {
        boost::mutex::scoped_lock lock(data.get_mutex());
        status = ippsAdd_64f_I(tmpdst, data.cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::cerr << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    data.inc_processed();
    return data.cdata();
}

/* Alloc */

Ipp16s *fft::alloc(Ipp16s *d, int length) {
    d = ippsMalloc_16s(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

Ipp32s *fft::alloc(Ipp32s *d, int length) {
    d = ippsMalloc_32s(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

Ipp32f *fft::alloc(Ipp32f *d, int length) {
    d = ippsMalloc_32f(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

Ipp64f *fft::alloc(Ipp64f *d, int length) {
    d = ippsMalloc_64f(length);
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

Ipp32sc *fft::alloc(Ipp32sc *d, int length) {
    d = ippsMalloc_32sc(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

Ipp32fc *fft::alloc(Ipp32fc *d, int length) {
    d = ippsMalloc_32fc(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

Ipp64fc *fft::alloc(Ipp64fc *d, int length) {
    d = ippsMalloc_64fc(length);
    if( d == NULL ) {
        std::cerr << "Not enough memory" << std::endl;
        exit(3);
    }
    return d;
}

/* Zero mem */

void fft::zero_mem(Ipp16s *d, int length) {
    ippsZero_16s(d, length);
}

void fft::zero_mem(Ipp32s *d, int length) {
    ippsZero_32s(d, length);
}

void fft::zero_mem(Ipp32f *d, int length) {
    ippsZero_32f(d, length);
}

void fft::zero_mem(Ipp64f *d, int length) {
    ippsZero_64f(d, length);
}

void fft::zero_mem(Ipp16sc *d, int length) {
    ippsZero_16sc(d, length);
}

void fft::zero_mem(Ipp32sc *d, int length) {
    ippsZero_32sc(d, length);
}

void fft::zero_mem(Ipp32fc *d, int length) {
    ippsZero_32fc(d, length);
}

void fft::zero_mem(Ipp64fc *d, int length) {
    ippsZero_64fc(d, length);
}

/* Other funcs */

int fft::order_to_length(int order) {
    return boost::numeric_cast<int>( pow(2, order) );
}
