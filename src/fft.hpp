#ifndef __SPECTRA2_FFT_HPP_
#define __SPECTRA2_FFT_HPP_
#include <boost/thread/mutex.hpp>
#include <ipp.h>
#include <stack>
#include "fft_buf.hpp"

class fft {
    public:
        /* Constructors */
        fft(const IppsFFTSpec_R_16s *spec);
        fft(const IppsFFTSpec_R_32s *spec);
        fft(const IppsFFTSpec_R_32f *spec);
        fft(const IppsFFTSpec_R_64f *spec);

        /* AllocSpec */
        static IppsFFTSpec_R_16s *allocSpec(IppsFFTSpec_R_16s **spec, int order, bool fast = false);
        static IppsFFTSpec_R_32s *allocSpec(IppsFFTSpec_R_32s **spec, int order, bool fast = false);
        static IppsFFTSpec_R_32f *allocSpec(IppsFFTSpec_R_32f **spec, int order, bool fast = false);
        static IppsFFTSpec_R_64f *allocSpec(IppsFFTSpec_R_64f **spec, int order, bool fast = false);

        /* Transforms */
        Ipp16s *transform(const SrcType<Ipp16s> & src, FFTBuf<Ipp16s> & dst, int order, int scaling = 1, int pscaling = 12);
        //FIXME: Missing functions?
        //Ipp32s *transform(const SrcType<Ipp32s> & src, FFTBuf<Ipp32s> & dst, int order, int scaling = 1, int pscaling = 12);
        Ipp32f *transform(const SrcType<Ipp16s> & src, FFTBuf<Ipp32f> & dst, int order, int scaling = 1, int pscaling = 12);
        Ipp32f *transform(const SrcType<Ipp32f> & src, FFTBuf<Ipp32f> & dst, int order, int scaling = 1, int pscaling = 12);
        Ipp64f *transform(const SrcType<Ipp64f> & src, FFTBuf<Ipp64f> & dst, int order, int scaling = 1, int pscaling = 12);

        /* Alloc */
        static Ipp8u *alloc(Ipp8u *d, int length);
        static Ipp16s *alloc(Ipp16s *d, int length);
        static Ipp32s *alloc(Ipp32s *d, int length);
        static Ipp32f *alloc(Ipp32f *d, int length);
        static Ipp64f *alloc(Ipp64f *d, int length);
        static Ipp16sc *alloc(Ipp16sc *d, int length);
        static Ipp32sc *alloc(Ipp32sc *d, int length);
        static Ipp32fc *alloc(Ipp32fc *d, int length);
        static Ipp64fc *alloc(Ipp64fc *d, int length);

        /* Zero mem */
        static void zero_mem(Ipp16s *d, int length);
        static void zero_mem(Ipp32s *d, int length);
        static void zero_mem(Ipp32f *d, int length);
        static void zero_mem(Ipp64f *d, int length);
        static void zero_mem(Ipp16sc *d, int length);
        static void zero_mem(Ipp32sc *d, int length);
        static void zero_mem(Ipp32fc *d, int length);
        static void zero_mem(Ipp64fc *d, int length);

        /* Free */
        static void free(void *p) { ippsFree(p); }

        /* Other funcs */
        static int order_to_length(int order);
    private:
        int _bufsize;
        std::stack<Ipp8u *> _buffers;
        boost::mutex _bufmut;

        /* FFTSpec */
        //const IppsFFTSpec FFTSpec;
        union {
            const IppsFFTSpec_R_16s *R_16s;
            const IppsFFTSpec_R_32s *R_32s;
            const IppsFFTSpec_R_32f *R_32f;
            const IppsFFTSpec_R_64f *R_64f;
        };

        Ipp8u *_get_buffer();
        void _release_buffer(Ipp8u *buf);
};
#endif
