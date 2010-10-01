#ifndef __SPECTRA2_IPP_HPP_
#define __SPECTRA2_IPP_HPP_
#include <ipp.h>

class IPP {
    public:
        /* AllocSpec */
        static IppsFFTSpec_R_16s *allocSpec(IppsFFTSpec_R_16s **spec, int order, bool fast = false);
        static IppsFFTSpec_R_32s *allocSpec(IppsFFTSpec_R_32s **spec, int order, bool fast = false);
        static IppsFFTSpec_R_32f *allocSpec(IppsFFTSpec_R_32f **spec, int order, bool fast = false);
        static IppsFFTSpec_R_64f *allocSpec(IppsFFTSpec_R_64f **spec, int order, bool fast = false);

        /* Alloc */
        static Ipp8u *alloc(Ipp8u *d, long int length);
        static Ipp16s *alloc(Ipp16s *d, long int length);
        static Ipp32s *alloc(Ipp32s *d, long int length);
        static Ipp32f *alloc(Ipp32f *d, long int length);
        static Ipp64f *alloc(Ipp64f *d, long int length);
        static Ipp16sc *alloc(Ipp16sc *d, long int length);
        static Ipp32sc *alloc(Ipp32sc *d, long int length);
        static Ipp32fc *alloc(Ipp32fc *d, long int length);
        static Ipp64fc *alloc(Ipp64fc *d, long int length);

        /* Zero mem */
        static void zero_mem(Ipp16s *d, long int length);
        static void zero_mem(Ipp32s *d, long int length);
        static void zero_mem(Ipp32f *d, long int length);
        static void zero_mem(Ipp64f *d, long int length);
        static void zero_mem(Ipp16sc *d, long int length);
        static void zero_mem(Ipp32sc *d, long int length);
        static void zero_mem(Ipp32fc *d, long int length);
        static void zero_mem(Ipp64fc *d, long int length);

        /* Free */
        static void free(void *p) { ippsFree(p); }

        /* Add */
        static Ipp16s *add(Ipp16s *dst, Ipp16s *addend, long int length);
        static Ipp32f *add(Ipp32f *dst, Ipp32f *addend, long int length);
        static Ipp64f *add(Ipp64f *dst, Ipp64f *addend, long int length);

        /* Other funcs */
        static long int order_to_length(int order);
};
#endif
