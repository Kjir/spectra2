#ifndef __SPECTRA2_DATA_LENGTH_HPP_
#define __SPECTRA2_DATA_LENGTH_HPP_
#include <boost/shared_ptr.hpp>
#include "fft_buf.hpp"
#include "type.hpp"

/* CONSTANTS to define data type.
 * Change this & recompile to change data types!
 */

const int IPP_DATA_LENGTH = 16;
const int IPP_OUTPUT_DATA_LENGTH = 32;
const bool IS_COMPLEX_TYPE = false;
typedef typer<IPP_DATA_LENGTH, IS_COMPLEX_TYPE>::type IppType;
typedef typer<IPP_OUTPUT_DATA_LENGTH, IS_COMPLEX_TYPE>::type DstIppType;

typedef boost::shared_ptr< FFTBuf<DstIppType> > FFTBufPtr;
#endif /* __SPECTRA2_DATA_LENGTH_HPP_ */
