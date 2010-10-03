#include <boost/numeric/conversion/cast.hpp>
#include <math.h>
#include <sstream>
#include "ipp.hpp"
#include "debug.hpp"

/* AllocSpec */

IppsFFTSpec_R_16s *IPP::allocSpec(IppsFFTSpec_R_16s **spec, int order, bool fast) {
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
        error(ss);
        exit(1);
    }

    return *spec;
}

IppsFFTSpec_R_32s *IPP::allocSpec(IppsFFTSpec_R_32s **spec, int order, bool fast) {
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
        error(ss);
        exit(1);
    }

    return *spec;
}

IppsFFTSpec_R_32f *IPP::allocSpec(IppsFFTSpec_R_32f **spec, int order, bool fast) {
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
        error(ss);
        exit(1);
    }

    return *spec;
}

IppsFFTSpec_R_64f *IPP::allocSpec(IppsFFTSpec_R_64f **spec, int order, bool fast) {
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
        error(ss);
        exit(1);
    }

    return *spec;
}

/* Alloc */

Ipp8u *IPP::alloc(Ipp8u *d, long int length) {
    d = ippsMalloc_8u(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp16s *IPP::alloc(Ipp16s *d, long int length) {
    d = ippsMalloc_16s(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp32s *IPP::alloc(Ipp32s *d, long int length) {
    d = ippsMalloc_32s(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp32f *IPP::alloc(Ipp32f *d, long int length) {
    d = ippsMalloc_32f(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp64f *IPP::alloc(Ipp64f *d, long int length) {
    d = ippsMalloc_64f(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp16sc *IPP::alloc(Ipp16sc *d, long int length) {
    d = ippsMalloc_16sc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp32sc *IPP::alloc(Ipp32sc *d, long int length) {
    d = ippsMalloc_32sc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp32fc *IPP::alloc(Ipp32fc *d, long int length) {
    d = ippsMalloc_32fc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

Ipp64fc *IPP::alloc(Ipp64fc *d, long int length) {
    d = ippsMalloc_64fc(length);
    if( d == NULL ) {
        std::stringstream ss;
        ss << "Not enough memory" << std::endl;
        error(ss);
        exit(3);
    }
    return d;
}

/* Zero mem */

void IPP::zero_mem(Ipp16s *d, long int length) {
    ippsZero_16s(d, length);
}

void IPP::zero_mem(Ipp32s *d, long int length) {
    ippsZero_32s(d, length);
}

void IPP::zero_mem(Ipp32f *d, long int length) {
    ippsZero_32f(d, length);
}

void IPP::zero_mem(Ipp64f *d, long int length) {
    ippsZero_64f(d, length);
}

void IPP::zero_mem(Ipp16sc *d, long int length) {
    ippsZero_16sc(d, length);
}

void IPP::zero_mem(Ipp32sc *d, long int length) {
    ippsZero_32sc(d, length);
}

void IPP::zero_mem(Ipp32fc *d, long int length) {
    ippsZero_32fc(d, length);
}

void IPP::zero_mem(Ipp64fc *d, long int length) {
    ippsZero_64fc(d, length);
}

/* Add */

Ipp16s *IPP::add(Ipp16s *addend, Ipp16s *dst, long int length) {
    IppStatus status = ippsAdd_16s_I(addend, dst, length);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }
    return dst;
}

Ipp32f *IPP::add(Ipp32f *addend, Ipp32f *dst, long int length) {
    IppStatus status = ippsAdd_32f_I(addend, dst, length);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }
    return dst;
}

Ipp64f *IPP::add(Ipp64f *addend, Ipp64f *dst, long int length) {
    IppStatus status = ippsAdd_64f_I(addend, dst, length);
    if( status != ippStsNoErr ) {
        std::stringstream ss;
        ss << "IPP Error in Add: " << ippGetStatusString(status) << "\n";
        error(ss);
        exit(7);
    }
    return dst;
}

/* Other funcs */

long int IPP::order_to_length(int order) {
    return boost::numeric_cast<long int>( pow(2, order) );
}

