#include "fft.hpp"
#include <iostream>
#include <stdlib.h>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/thread/mutex.hpp>
#include <math.h>
#include <sstream>
#include "debug.hpp"

/* Constructors */

fft::fft(const IppsFFTSpec_R_16s *spec) : R_16s(spec) {
    IppStatus status;

    status = ippsFFTGetBufSize_R_16s( spec, &_bufsize );
    if( status != ippStsNoErr ) {
    std::stringstream ss;
        ss << "IPP Error in FFTGetBufSize: " << ippGetStatusString(status) << std::endl;
        debug(ss.str());
        exit(2);
    }
    /*
    buffer = ippsMalloc_8u(bufsize); //TODO: Create some buffer handling thingy
    if( buffer == NULL ) {
    std::stringstream ss;
        ss << "Not enough memory\n";
        debug(ss.str());
        exit(3);
    }
    */
}

fft::fft(const IppsFFTSpec_R_32s *spec) : R_32s(spec) {}
fft::fft(const IppsFFTSpec_R_32f *spec) : R_32f(spec) {}
fft::fft(const IppsFFTSpec_R_64f *spec) : R_64f(spec) {}

/* AllocSpec */

IppsFFTSpec_R_16s *fft::allocSpec(IppsFFTSpec_R_16s **spec, int order, bool fast) {
    IppStatus status;
    IppHintAlgorithm hint;

    /*
     * Choose the algorithm type: fast or accurate
     */
    if(fast) {
        hint = ippAlgHintFast;
        std::stringstream ss;
        ss << "Using fast algo" << std::endl;
        debug(ss.str());
    } else {
        hint = ippAlgHintAccurate;
    }

    status = ippsFFTInitAlloc_R_16s(spec, order, IPP_FFT_DIV_BY_SQRTN, hint);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
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
        std::stringstream ss;
        ss << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
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
        std::stringstream ss;
        ss << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
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
        std::stringstream ss;
        ss << "IPP Error in InitAlloc: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(1);
    }

    return *spec;
}

/* Transform */

Ipp16s *fft::transform(const SrcType<Ipp16s> &src, boost::shared_ptr<FFTBuf<Ipp16s> > data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp16s *tmpdst;
    long int siglen = fft::order_to_length(order);
    Ipp8u *buffer = _get_buffer();
    
    tmpdst = fft::alloc(data->cdata(), siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src.data, tmpdst, R_16s, scaling, buffer);
    _release_buffer(buffer);
    buffer = NULL;
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(4);
    }
    Ipp16sc *vc;
    vc = fft::alloc(vc, siglen);
    if( vc == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory\n";
        debug(ss.str());
        exit(3);
    }
    //Set the vector to zero
    fft::zero_mem(vc, siglen);
    status = ippsConjPack_16sc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(5);
    }
    status = ippsPowerSpectr_16sc_Sfs(vc, tmpdst, siglen, pscaling);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(6);
    }
    fft::free(vc);
    vc = NULL;

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_16s_I(tmpdst, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    data->inc_processed();
    return data->cdata();
}

Ipp32f *fft::transform(const SrcType<Ipp16s> &src, boost::shared_ptr< FFTBuf<Ipp32f> > data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp16s *tmpdst;
    long int siglen = fft::order_to_length(order);
    Ipp8u *buffer = _get_buffer();

    tmpdst = fft::alloc(tmpdst, siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src.data, tmpdst, R_16s, scaling, buffer);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(4);
    }
    _release_buffer(buffer);
    buffer = NULL;
    Ipp16sc *vc;
    vc = fft::alloc(vc, siglen);
    //Set the vector to zero
    fft::zero_mem(vc, siglen);
    status = ippsConjPack_16sc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(5);
    }
    Ipp32f *dst32 = fft::alloc(dst32, siglen);
    status = ippsPowerSpectr_16sc32f(vc, dst32, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(6);
    }
    fft::free(vc);
    vc = NULL;

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_32f_I(dst32, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(dst32);
    dst32 = NULL;
    fft::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    dst32 = data->cdata();
    data->inc_processed();
    return dst32;
}

Ipp32f *fft::transform(const SrcType<Ipp32f> &src, boost::shared_ptr<FFTBuf<Ipp32f> > data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp32f *tmpdst;
    long int siglen = fft::order_to_length(order);
    Ipp8u *buffer = _get_buffer();

    tmpdst = fft::alloc(data->cdata(), siglen);

    status = ippsFFTFwd_RToPack_32f(src.data, tmpdst, R_32f, buffer);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(4);
    }
    _release_buffer(buffer);
    buffer = NULL;
    Ipp32fc *vc;
    vc = fft::alloc(vc, siglen);
    if( vc == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory\n";
        debug(ss.str());
        exit(3);
    }
    //Set the vector to zero
    fft::zero_mem(vc, siglen);
    status = ippsConjPack_32fc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(5);
    }
    status = ippsPowerSpectr_32fc(vc, tmpdst, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(6);
    }
    fft::free(vc);
    vc = NULL;

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_32f_I(tmpdst, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    data->inc_processed();
    return data->cdata();
}

Ipp64f *fft::transform(const SrcType<Ipp64f> &src, boost::shared_ptr<FFTBuf<Ipp64f> > data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp64f *tmpdst;
    Ipp8u *buffer = _get_buffer();
    long int siglen = fft::order_to_length(order);

    tmpdst = fft::alloc(data->cdata(), siglen);

    status = ippsFFTFwd_RToPack_64f(src.data, tmpdst, R_64f, buffer);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(4);
    }
    _release_buffer(buffer);
    buffer = NULL;
    Ipp64fc *vc;
    vc = fft::alloc(vc, siglen);
    if( vc == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory\n";
        debug(ss.str());
        exit(3);
    }
    //Set the vector to zero
    fft::zero_mem(vc, siglen);
    status = ippsConjPack_64fc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(5);
    }
    status = ippsPowerSpectr_64fc(vc, tmpdst, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(6);
    }
    fft::free(vc);
    vc = NULL;

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_64f_I(tmpdst, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        debug(ss.str());
        exit(7);
    }

    fft::free(tmpdst);
    tmpdst = NULL;
    fft::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    data->inc_processed();
    return data->cdata();
}

/* Alloc */

Ipp8u *fft::alloc(Ipp8u *d, long int length) {
    d = ippsMalloc_8u(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp16s *fft::alloc(Ipp16s *d, long int length) {
    d = ippsMalloc_16s(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp32s *fft::alloc(Ipp32s *d, long int length) {
    d = ippsMalloc_32s(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp32f *fft::alloc(Ipp32f *d, long int length) {
    d = ippsMalloc_32f(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp64f *fft::alloc(Ipp64f *d, long int length) {
    d = ippsMalloc_64f(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp16sc *fft::alloc(Ipp16sc *d, long int length) {
    d = ippsMalloc_16sc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp32sc *fft::alloc(Ipp32sc *d, long int length) {
    d = ippsMalloc_32sc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp32fc *fft::alloc(Ipp32fc *d, long int length) {
    d = ippsMalloc_32fc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

Ipp64fc *fft::alloc(Ipp64fc *d, long int length) {
    d = ippsMalloc_64fc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        debug(ss.str());
        exit(3);
    }
    return d;
}

/* Zero mem */

void fft::zero_mem(Ipp16s *d, long int length) {
    ippsZero_16s(d, length);
}

void fft::zero_mem(Ipp32s *d, long int length) {
    ippsZero_32s(d, length);
}

void fft::zero_mem(Ipp32f *d, long int length) {
    ippsZero_32f(d, length);
}

void fft::zero_mem(Ipp64f *d, long int length) {
    ippsZero_64f(d, length);
}

void fft::zero_mem(Ipp16sc *d, long int length) {
    ippsZero_16sc(d, length);
}

void fft::zero_mem(Ipp32sc *d, long int length) {
    ippsZero_32sc(d, length);
}

void fft::zero_mem(Ipp32fc *d, long int length) {
    ippsZero_32fc(d, length);
}

void fft::zero_mem(Ipp64fc *d, long int length) {
    ippsZero_64fc(d, length);
}

/* Other funcs */

long int fft::order_to_length(int order) {
    return boost::numeric_cast<long int>( pow(2, order) );
}

Ipp8u *fft::_get_buffer() {
    boost::mutex::scoped_lock lock(_bufmut);
    Ipp8u *buf;
    if(_buffers.empty()) {
        buf = fft::alloc(buf, _bufsize);

        std::stringstream ss;
        ss << "Created new buffer" << std::endl;
        debug(ss.str());
    } else {
        buf = _buffers.top();
        _buffers.pop();
    }
    return buf;
}

void fft::_release_buffer(Ipp8u *buf) {
    boost::mutex::scoped_lock lock(_bufmut);
    _buffers.push(buf);
}
