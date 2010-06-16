#ifndef __SPECTRA2_FFT_HPP_
#define __SPECTRA2_FFT_HPP_
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
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

        /* Transforms */
        Ipp16s *transform(const SrcType<Ipp16s> & src, boost::shared_ptr<FFTBuf<Ipp16s> > dst, int order, int scaling = 1, int pscaling = 12);
        //FIXME: Missing functions?
        //Ipp32s *transform(const SrcType<Ipp32s> & src, FFTBuf<Ipp32s> & dst, int order, int scaling = 1, int pscaling = 12);
        Ipp32f *transform(const SrcType<Ipp16s> & src, boost::shared_ptr< FFTBuf<Ipp32f> > dst, int order, int scaling = 1, int pscaling = 12);
        Ipp32f *transform(const SrcType<Ipp32f> & src, boost::shared_ptr<FFTBuf<Ipp32f> > dst, int order, int scaling = 1, int pscaling = 12);
        Ipp64f *transform(const SrcType<Ipp64f> & src, boost::shared_ptr<FFTBuf<Ipp64f> > dst, int order, int scaling = 1, int pscaling = 12);
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
