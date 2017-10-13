#ifndef __SPECTRA2_SINK_HPP_
#define __SPECTRA2_SINK_HPP_

#include <stddef.h>

class SinkFilter {
    public:
        virtual ~SinkFilter() {};
        virtual void write(void *data, size_t size) = 0;
        virtual void flush() = 0;
};

#endif /* __SPECTRA2_SINK_HPP_ */
