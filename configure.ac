AC_PREREQ(2.61)
AC_INIT([spectra2],[0.2])
AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE([-Wall])

AC_CANONICAL_HOST

AC_SEARCH_LIBS([strerror],[cposix])
AC_PROG_CXX
AC_LANG([C++])

#Check for libraries
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday pow])

#Check for Boost
AX_BOOST_BASE([1.36.0])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_THREAD
AX_BOOST_THREADPOOL
AX_BOOST_ASIO

#Checks for Intel Integrated Performance Primitives
AG_CHECK_IPP

if test "x$HAVE_IPP" = "xfalse"; then
	AC_MSG_ERROR(***Integrated Performance Primitives could not be found***)
else
	IPP_INCLUDES=-I$IPP_PREFIX/$ver/$IPP_CPU/include
    IPP_LDFLAGS="-L$IPP_PREFIX/$ver/$IPP_CPU/sharedlib -L$IPP_PREFIX/$ver/$IPP_CPU/lib -Wl,-R$IPP_PREFIX/$ver/$IPP_CPU/sharedlib"
	IPP_LIBS="-lippac$IPP_SUFFIX -lipps$IPP_SUFFIX -lippcore$IPP_SUFFIX -liomp5 -lpthread"
fi
AC_SUBST(IPP_INCLUDES)
AC_SUBST(IPP_LDFLAGS)
AC_SUBST(IPP_LIBS)

AC_CONFIG_FILES([Makefile test/Makefile src/Makefile])

AC_OUTPUT
