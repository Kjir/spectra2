#ifndef __SPECTRA2_SOURCE_HPP_
#define __SPECTRA2_SOURCE_HPP_

#include <stddef.h>

class SourceFilter {
    public:
        virtual ~SourceFilter() {};
        virtual void *read(void *destination, size_t size) = 0;
};

#endif /* __SPECTRA2_SOURCE_HPP_ */
