#ifndef __SPECTRA2_OUTPUT_HPP_
#define __SPECTRA2_OUTPUT_HPP_

#include <boost/shared_ptr.hpp>
#include "list.hpp"
#include "fft_buf.hpp"
#include "filter/sink.hpp"
#include <ipp.h>

void output(List< boost::shared_ptr< FFTBuf<Ipp32f> > > &l, SinkFilter &s);
#endif
