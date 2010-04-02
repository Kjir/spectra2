#ifndef __SPECTRA2_FFT_HPP_
#define __SPECTRA2_FFT_HPP_
#include <ipp.h>
#include "fft_buf.hpp"

class fft {
    public:
        fft(const IppsFFTSpec_R_16s *spec);
        static IppsFFTSpec_R_16s *allocSpec(IppsFFTSpec_R_16s **spec, int order, bool fast = false);
        Ipp16s *transform(const SrcType<Ipp16s> & src, FFTBuf<Ipp16s> & dst, int order, int scaling = 1, int pscaling = 12);
        static Ipp16s *alloc(Ipp16s *d, int length);
        static Ipp16sc *alloc(Ipp16sc *d, int length);
        static int order_to_length(int order);
        static void zero_mem(Ipp16s *d, int length);
        static void free(Ipp16s *p) { ippsFree(p); }
        static void free(Ipp16sc *cp) { ippsFree(cp); }
    private:
        Ipp8u *buffer;
        const IppsFFTSpec_R_16s *FFTSpec;
};
#endif
