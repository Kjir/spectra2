#include "fft.hpp"
#include "ipp.hpp"
#include <stdlib.h>
#include <boost/thread/mutex.hpp>
#include <sstream>
#include "debug.hpp"

/* Constructors */

fft::fft(const IppsFFTSpec_R_16s *spec) : R_16s(spec) {
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

fft::fft(const IppsFFTSpec_R_32s *spec) : R_32s(spec) {}
fft::fft(const IppsFFTSpec_R_32f *spec) : R_32f(spec) {}
fft::fft(const IppsFFTSpec_R_64f *spec) : R_64f(spec) {}

/* Transform */

Ipp16s *fft::transform(const SrcType<Ipp16s> &src, boost::shared_ptr<FFTBuf<Ipp16s> > data, int order, int scaling, int pscaling) {
    IppStatus status;
    Ipp16s *tmpdst;
    long int siglen = IPP::order_to_length(order);
    Ipp8u *buffer = _get_buffer();
    
    tmpdst = IPP::alloc(data->cdata(), siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src.data, tmpdst, R_16s, scaling, buffer);
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

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_16s_I(tmpdst, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    IPP::free(src.data);
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
    long int siglen = IPP::order_to_length(order);
    Ipp8u *buffer = _get_buffer();

    tmpdst = IPP::alloc(tmpdst, siglen);

    status = ippsFFTFwd_RToPack_16s_Sfs(src.data, tmpdst, R_16s, scaling, buffer);
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

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_32f_I(dst32, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    IPP::free(dst32);
    dst32 = NULL;
    IPP::free(src.data);
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
    long int siglen = IPP::order_to_length(order);
    Ipp8u *buffer = _get_buffer();

    tmpdst = IPP::alloc(data->cdata(), siglen);

    status = ippsFFTFwd_RToPack_32f(src.data, tmpdst, R_32f, buffer);
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

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_32f_I(tmpdst, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    IPP::free(src.data);
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
    long int siglen = IPP::order_to_length(order);

    tmpdst = IPP::alloc(data->cdata(), siglen);

    status = ippsFFTFwd_RToPack_64f(src.data, tmpdst, R_64f, buffer);
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

    {
        boost::mutex::scoped_lock lock(data->get_mutex());
        status = ippsAdd_64f_I(tmpdst, data->cdata(), siglen);
    }
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }

    IPP::free(tmpdst);
    tmpdst = NULL;
    IPP::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    data->inc_processed();
    return data->cdata();
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
