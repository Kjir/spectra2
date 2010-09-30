#ifndef __SPECTRA2_MERGE_HPP_
#define __SPECTRA2_MERGE_HPP_

#include "data_length.hpp"
#include "fft_buf.hpp"

class MergeFilter {
    public:
        virtual ~FilterChain() {};
        virtual void merge(SrcType<IppType> &src, FFTBufPtr dst, DstIppType *result) = 0;
}

#endif /* __SPECTRA2_MERGE_HPP_ */
