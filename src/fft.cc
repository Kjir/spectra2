#include "data_length.hpp"
#include "debug.hpp"
#include "fft.hpp"
#include "ipp.hpp"
#include <boost/thread/mutex.hpp>
#include <sstream>
#include <stdlib.h>

/* Constructors */

fft::fft(const IppsFFTSpec_R_16s *spec, int order, int scaling, int pscaling) : R_16s(spec), _order(order), _scaling(scaling), _pscaling(pscaling) {
    IppStatus status;

    status = ippsFFTGetBufSize_R_16s( spec, &_bufsize );
    if( status != ippStsNoErr ) {
    std::stringstream ss;
        ss << "IPP Error in FFTGetBufSize: " << ippGetStatusString(status) << std::endl;
        error(ss);
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

fft::fft(const IppsFFTSpec_R_32s *spec, int order, int scaling, int pscaling) : R_32s(spec), _order(order), _scaling(scaling), _pscaling(pscaling) {}
fft::fft(const IppsFFTSpec_R_32f *spec, int order, int scaling, int pscaling) : R_32f(spec), _order(order), _scaling(scaling), _pscaling(pscaling) {}
fft::fft(const IppsFFTSpec_R_64f *spec, int order, int scaling, int pscaling) : R_64f(spec), _order(order), _scaling(scaling), _pscaling(pscaling) {}

/* Interface implementation */
DstIppType *fft::transform(const IppType *original_signal, DstIppType *current_signal)
{
    return transform(original_signal, current_signal, _order, _scaling, _pscaling);
}

/* Transform */

Ipp16s *fft::transform(const Ipp16s *src, Ipp16s *data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp16s *tmpdst;
    long int siglen = IPP::order_to_length(order);
    Ipp8u *buffer = _get_buffer();
    
    tmpdst = IPP::alloc(tmpdst, siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src, tmpdst, R_16s, scaling, buffer);
    _release_buffer(buffer);
    buffer = NULL;
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(4);
    }
    Ipp16sc *vc;
    vc = IPP::alloc(vc, siglen);
    //Set the vector to zero
    IPP::zero_mem(vc, siglen);
    status = ippsConjPack_16sc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(5);
    }
    status = ippsPowerSpectr_16sc_Sfs(vc, tmpdst, siglen, pscaling);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(6);
    }
    IPP::free(vc);
    vc = NULL;

    status = ippsCopy_16s(tmpdst, data, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Copy: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    return data;
}

Ipp32f *fft::transform(const Ipp16s *src, Ipp32f *data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp16s *tmpdst;
    long int siglen = IPP::order_to_length(order);
    Ipp8u *buffer = _get_buffer();

    tmpdst = IPP::alloc(tmpdst, siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src, tmpdst, R_16s, scaling, buffer);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(4);
    }
    _release_buffer(buffer);
    buffer = NULL;
    Ipp16sc *vc;
    vc = IPP::alloc(vc, siglen);
    //Set the vector to zero
    IPP::zero_mem(vc, siglen);
    status = ippsConjPack_16sc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(5);
    }
    Ipp32f *dst32 = IPP::alloc(dst32, siglen);
    status = ippsPowerSpectr_16sc32f(vc, dst32, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(6);
    }
    IPP::free(vc);
    vc = NULL;

    status = ippsCopy_32f(dst32, data, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Copy: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    IPP::free(dst32);
    dst32 = NULL;
    return data;
}

Ipp32f *fft::transform(const Ipp32f *src, Ipp32f *data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp32f *tmpdst;
    long int siglen = IPP::order_to_length(order);
    Ipp8u *buffer = _get_buffer();

    tmpdst = IPP::alloc(tmpdst, siglen);

    status = ippsFFTFwd_RToPack_32f(src, tmpdst, R_32f, buffer);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(4);
    }
    _release_buffer(buffer);
    buffer = NULL;
    Ipp32fc *vc;
    vc = IPP::alloc(vc, siglen);
    //Set the vector to zero
    IPP::zero_mem(vc, siglen);
    status = ippsConjPack_32fc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(5);
    }
    status = ippsPowerSpectr_32fc(vc, tmpdst, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(6);
    }
    IPP::free(vc);
    vc = NULL;

    status = ippsCopy_32f(tmpdst, data, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Copy: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    return data;
}

Ipp64f *fft::transform(const Ipp64f *src, Ipp64f *data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp64f *tmpdst;
    Ipp8u *buffer = _get_buffer();
    long int siglen = IPP::order_to_length(order);

    tmpdst = IPP::alloc(tmpdst, siglen);

    status = ippsFFTFwd_RToPack_64f(src, tmpdst, R_64f, buffer);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in FFTFwd: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(4);
    }
    _release_buffer(buffer);
    buffer = NULL;
    Ipp64fc *vc;
    vc = IPP::alloc(vc, siglen);
    //Set the vector to zero
    IPP::zero_mem(vc, siglen);
    status = ippsConjPack_64fc(tmpdst, vc, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in ConjPack: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(5);
    }
    status = ippsPowerSpectr_64fc(vc, tmpdst, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in PowerSpectr: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(6);
    }
    IPP::free(vc);
    vc = NULL;

    status = ippsCopy_64f(tmpdst, data, siglen);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Copy: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    return data;
}

Ipp8u *fft::_get_buffer() {
    boost::mutex::scoped_lock lock(_bufmut);
    Ipp8u *buf;
    if(_buffers.empty()) {
        buf = IPP::alloc(buf, _bufsize);

        std::stringstream ss;
        ss << "Created new buffer" << std::endl;
        debug(ss);
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
