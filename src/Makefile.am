bin_PROGRAMS = fftserver fftnoout fftdebug

fftserver_SOURCES = fft.cc main.cc output.cc debug.cc ipp.cc file.cc ipp/AddMerger.cc filter/chain.cc
fftserver_CPPFLAGS=$(IPP_INCLUDES) $(BOOST_CPPFLAGS)
fftserver_LDFLAGS=$(IPP_LDFLAGS) $(BOOST_LDFLAGS)
fftserver_LDADD = $(IPP_LIBS) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) $(BOOST_THREAD_LIB) -lm

fftnoout_SOURCES = fft.cc main.cc noout.cc debug.cc ipp.cc file.cc ipp/AddMerger.cc filter/chain.cc
fftnoout_CPPFLAGS=$(IPP_INCLUDES) $(BOOST_CPPFLAGS)
fftnoout_LDFLAGS=$(IPP_LDFLAGS) $(BOOST_LDFLAGS)
fftnoout_LDADD = $(IPP_LIBS) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) $(BOOST_THREAD_LIB) -lm

fftdebug_SOURCES = fft.cc main.cc output.cc debug.cc ipp.cc file.cc ipp/AddMerger.cc filter/chain.cc
fftdebug_CPPFLAGS=$(IPP_INCLUDES) $(BOOST_CPPFLAGS) -DDEBUG
fftdebug_LDFLAGS=$(IPP_LDFLAGS) $(BOOST_LDFLAGS)
fftdebug_LDADD = $(IPP_LIBS) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) $(BOOST_THREAD_LIB) -lm
