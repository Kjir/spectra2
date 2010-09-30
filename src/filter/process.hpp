#ifndef __SPECTRA2_PROCESS_HPP_
#define __SPECTRA2_PROCESS_HPP_

#include <stddef.h>
#include "data_length.hpp"
#include "fft_buf.hpp"

class ProcessFilter {
    public:
        virtual ~ProcessFilter() {};
        virtual DstIppType *transform(const IppType *original_signal, DstIppType *current_signal) = 0;
};

#endif /* __SPECTRA2_PROCESS_HPP_ */
