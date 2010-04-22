#ifndef __SPECTRA2_ENDIAN_H_
#define __SPECTRA2_ENDIAN_H_
#ifdef HAVE_ENDIAN_H
#include <endian.h>
#elif defined(HAVE_SYS_ENDIAN_H)
#include <sys/endian.h>
#endif /* HAVE_ENDIAN_H */
#if !defined(be64toh)
# include <bits/byteswap.h>
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define be64toh(x) __bswap_64 (x)
# else
#  define be64toh(x) (x)
# endif /* __BYTE_ORDER == __LITTLE_ENDIAN */
#endif /* be64toh */
#endif /* __SPECTRA2_ENDIAN_H_ */
