#ifndef __SPECTRA2_ADDMERGER_HPP_
#define __SPECTRA2_ADDMERGER_HPP_

#include "data_length.hpp"
#include "filter/merge.hpp"

class AddMerger : public MergeFilter
{
    public:
        virtual void merge(const SrcType<IppType> &src, FFTBufPtr dst, DstIppType *result);
};
#endif /* __SPECTRA2_ADDMERGER_HPP_ */
