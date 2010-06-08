#ifndef __SPECTRA2_OUTPUT_HPP_
#define __SPECTRA2_OUTPUT_HPP_

#include "list.hpp"
#include <iostream>
#include "fft_buf.hpp"
#include <ipp.h>

void output(List<FFTBuf<Ipp32f> *> &l, std::ostream *s);
#endif
