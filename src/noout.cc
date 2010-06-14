#include "output.hpp"
#include <boost/thread/mutex.hpp>

void output(List< boost::shared_ptr< FFTBuf<Ipp32f> > > &l, std::ostream *s)
{
    while(true)
    {
        while(l.empty())
        {
            std::stringstream ss;
            ss << "Before empty wait" << std::endl;
            debug(ss.str());
            s->flush();
            l.wait();
            ss.clear(); ss.str("");
            ss << "After empty wait" << std::endl;
            debug(ss.str());
        }
        FFTBuf<Ipp32f> *f = l.front().get();

        if(f->is_written()) {
            std::stringstream ss;
            ss << "Already written, delete" << std::endl;
            debug(ss.str());
            continue;
        }

        f->wait_until_processed();

        if(!f->set_written())
        {
            continue;
        }
        {
            boost::mutex::scoped_lock lock(f->get_mutex());
        }
        std::stringstream ss;
        ss << "Done writing, deleting" << std::endl;
        debug(ss.str());
    }
    std::stringstream ss;
    ss << "Quit" << std::endl;
    debug(ss.str());
}
