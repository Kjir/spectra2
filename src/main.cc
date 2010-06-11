#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/program_options.hpp>
#include <boost/ref.hpp>
#include <boost/threadpool.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <fstream>
#include <iostream>
#include <ipp.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <typeinfo>
#include "server.hpp"
#include "fft.hpp"
#include "fft_buf.hpp"
#include "list.hpp"
#include "output.hpp"
#include "type.hpp"
#include "debug.hpp"

int main(int argc, char **argv)
{
    /* CONSTANTS to define data type. Change this & recompile to change data types! */

    const int IPP_DATA_LENGTH = 16;
    const int IPP_OUTPUT_DATA_LENGTH = 32;
    const bool IS_COMPLEX_TYPE = false;
    typedef typer<IPP_DATA_LENGTH, IS_COMPLEX_TYPE>::type IppType;
    typedef typer<IPP_OUTPUT_DATA_LENGTH, IS_COMPLEX_TYPE>::type DstIppType;

    typedef boost::shared_ptr< FFTBuf<DstIppType> > FFTBufPtr;

    namespace po = boost::program_options;
    po::variables_map var_map;

    //FFT
    int order, num_threads, sums, scaling, pscaling;
    //Output
    std::string ofile;
    //Server
    std::string host;
    int port;
    //Advanced
    bool fast;
    int threshold, low_threads;

    /* Generic options */
    po::options_description gen_opts("Generic options");
    gen_opts.add_options()
        ("help", "Print this help message")
        ("help-config", "Print help message for config file options")
        ;
    /* FFT options */
    po::options_description fft_opts("FFT options");
    fft_opts.add_options()
        ("scaling,s", po::value<int>(&scaling)->default_value(0), "Scaling value. The output will be multiplied by 2^-scaling")
        ("pscaling,p", po::value<int>(&pscaling)->default_value(12), "Convert the complex numbers into the power spectrum with the defined scaling")
        ("nthreads,t", po::value<int>(&num_threads)->default_value(5), "Number of threads to use for FFT computation.")
        ("order,l", po::value<int>(&order)->default_value(8), "Order of the transform. The input and output signal have a 2^order length")
        ("nint,n", po::value<int>(&sums)->default_value(1), "Number of integrations (sums) desired")
        ;

    /*Output options*/
    po::options_description out_opts("Output options");
    out_opts.add_options()
        ("output-file,o", po::value<std::string>(&ofile)->default_value("-"), "File to be used for data output, - for stdout (default)")
        ;

    /* Server options */
    po::options_description srv_opts("Server options");
    srv_opts.add_options()
        ("listen,L", po::value<std::string>(&host)->default_value("localhost"), "Ip address where we should listen for datagrams")
        ("port,P", po::value<int>(&port)->default_value(50000), "UDP port for the server")
        ;

    /* Advanced options */
    po::options_description adv_opts("Advanced options");
    adv_opts.add_options()
        ("fast", po::value<bool>(&fast)->default_value(false), "Use fast algorithm instead of accurate one")
        ("threshold", po::value<int>(&threshold)->default_value(5), "Threshold below which we lower the number of threads working on FFT")
        ("low-threads", po::value<int>(&low_threads)->default_value(2), "Number of threads to use when there is a low number of integrations")
        ;

    /* Command line */
    po::options_description cmd_line;
    cmd_line.add(gen_opts).add(fft_opts).add(out_opts).add(srv_opts);
    po::store(po::command_line_parser(argc, argv).options(cmd_line).run(), var_map);

    /* Configuration files */
    po::options_description cfg_file;
    cfg_file.add(gen_opts).add(fft_opts).add(srv_opts).add(adv_opts);

    std::string homecfg(getenv("HOME"));
    homecfg.append("/.spectra2rc");
    std::fstream home(homecfg.c_str(), std::fstream::in);
    po::store(po::parse_config_file(home, cfg_file), var_map);
    home.close();
    std::fstream etc("/etc/spectra2", std::fstream::in);
    po::store(po::parse_config_file(etc, cfg_file), var_map);
    etc.close();
    po::notify(var_map);

    if( var_map.count("help") )
    {
        std::cout << cmd_line << "\n";
        return 0;
    }

    if( var_map.count("help-config") )
    {
        std::cout << cfg_file << "\n";
        return 0;
    }

    if(sums < threshold && num_threads > low_threads)
    {
        //Lower the number of threads if we have few integrations
        num_threads = low_threads;
        std::stringstream ss;
        ss << "Number of threads set to " << num_threads << std::endl;
        debug(ss.str());
    }

    ippStaticInit();
    try
    {
        boost::threadpool::pool tp(num_threads);
        {
            std::stringstream ss;
            ss << "Started threadpool with " << num_threads << " threads" << std::endl;
            debug(ss.str());
        }

        int i = 0;
        int siglen = fft::order_to_length(order);
        List<FFTBufPtr> dst;
        boost::circular_buffer<SrcType<IppType> > cbuf(num_threads * 3);

        IppsFFTSpec_R_16s *spec = fft::allocSpec(&spec, order, fast);

        fft f(spec); //The FFT object

        {
            std::stringstream ss;
            ss << "Listening on " << host << " port " << port << std::endl;
            debug(ss.str());
        }

        //udp_sock<IppType> s(host, port); //The UDP server
        SourceFilter *src_filter = new udp_sock<IppType>(host, port);

        std::ostream *out = &std::cout;
        if( ofile != "-" )
        {
            out = new std::ofstream(ofile.c_str(), std::ofstream::out|std::ofstream::trunc|std::ofstream::binary);
        }
        boost::thread out_thread(output, boost::ref(dst), out);

        while(true)
        {
            SrcType<IppType> src;
            src.data = fft::alloc(src.data, siglen);
            src_filter->read(src.data, siglen); //try-catch for missed datagram
            if(!cbuf.empty())
            {
                boost::mutex::scoped_lock lock(*cbuf.front().mutex);
                while(cbuf.full() && !cbuf.front().erasable)
                {
                    lock.unlock();
                    tp.wait(num_threads/2);
                    lock.lock();
                    std::stringstream ss;
                    ss << "Circular buffer is too small!!!" << std::endl;
                    debug(ss.str());
                }
            }
            cbuf.push_back(src);

            if( !dst.empty() ) {
                std::stringstream ss;
                ss << "Back is " << std::hex << dst.back() << std::dec << std::endl;
                debug(ss.str());
            }
            if( dst.empty() || dst.back()->is_src_full() ) {
                FFTBufPtr b( new FFTBuf<DstIppType>(siglen, sums) );
                *b = fft::alloc(b->cdata(), siglen);
                fft::zero_mem(b->cdata(), siglen);
                dst.push_back(b);
            }
            if( !dst.empty() ) {
                std::stringstream ss;
                ss << "Back is " << std::hex << dst.back() << std::dec << std::endl;
                debug(ss.str());
            }
            FFTBufPtr fbuf = dst.back();
            boost::mutex::scoped_lock lock(fbuf->get_mutex());
            fbuf->inc_assigned_sources();
            tp.schedule(
                    boost::bind(
                        static_cast<DstIppType *(fft::*) (const SrcType<IppType> &, FFTBufPtr, int, int, int)>(&fft::transform), &f, boost::cref(cbuf.back()), fbuf, order, scaling, pscaling
                        )
                    );
        }

        out_thread.join();
        if( *out != std::cout ) {
            delete out;
        }

    }
    catch( std::exception &e )
    {
        std::stringstream ss;
        ss << "Ex: " << e.what() << std::endl;
        debug(ss.str());
    }

    return 0;
}
