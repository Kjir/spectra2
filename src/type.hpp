#ifndef __SPECTRA2__TYPE_HPP_
#define __SPECTRA2__TYPE_HPP_
#include <ipp.h>

template<int size, bool is_complex> struct typer;
template<> struct typer<16, false>
{
    typedef Ipp16s type;

};

template<> struct typer<16, true>
{
    typedef Ipp16sc type;

};

template<> struct typer<32, false>
{
    typedef Ipp32f type;

};

template<> struct typer<64, false>
{
    typedef Ipp64f type;

};

#endif /* __SPECTRA2__TYPE_HPP_ */
