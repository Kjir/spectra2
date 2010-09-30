#ifndef __SPECTRA2_FILE_HPP_
#define __SPECTRA2_FILE_HPP_
#include "filter/sink.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class FileSink : public SinkFilter
{
    public:
        FileSink(std::string &output_file);
        virtual void write(void *data, size_t length);
        virtual void flush();
        virtual ~FileSink();
    private:
        std::ostream *_out;
};
#endif /* __SPECTRA2_FILE_HPP_ */
