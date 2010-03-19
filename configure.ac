AC_PREREQ(2.61)
AC_INIT([spectra2],[0.2])
AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE([-Wall])

AC_CANONICAL_HOST

AC_SEARCH_LIBS([strerror],[cposix])
AC_PROG_CXX
AC_LANG([C++])

#Check for Boost
AX_BOOST_BASE([1.36.0])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_THREAD
AX_BOOST_ASIO

AG_CHECK_IPP

if test "x$HAVE_IPP" = "xfalse"; then
	AC_MSG_ERROR(***Integrated Performance Primitives could not be found***)
else
	IPP_INCLUDES=-I$IPP_PREFIX/$ver/$IPP_CPU/include
    IPP_LDFLAGS="-L$IPP_PREFIX/$ver/$IPP_CPU/sharedlib -L$IPP_PREFIX/$ver/$IPP_CPU/lib"
	IPP_LIBS="-lippac$IPP_SUFFIX -lipps$IPP_SUFFIX -lippcore$IPP_SUFFIX -lpthread"
fi
AC_SUBST(IPP_INCLUDES)
AC_SUBST(IPP_LDFLAGS)
AC_SUBST(IPP_LIBS)

AC_CONFIG_FILES([Makefile test/Makefile src/Makefile])

AC_OUTPUT
