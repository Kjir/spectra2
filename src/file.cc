#include "file.hpp"
#include <iostream>
#include <string>

FileSink::FileSink(std::string &output_file)
{
    _out = &std::cout;
    if( output_file != "-" )
    {
        _out = new std::ofstream(output_file.c_str(), std::ofstream::out|std::ofstream::trunc|std::ofstream::binary);
    }
}

FileSink::~FileSink()
{
        if( *_out != std::cout ) {
            delete _out;
        }
}

void FileSink::flush()
{
    _out->flush();
}

void FileSink::write(void *data, size_t length)
{
    _out->write((char *)data, length);
}
