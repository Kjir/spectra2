#include "output.hpp"
#include <boost/thread/mutex.hpp>

void output(List<FFTBuf<Ipp32f> *> &l, std::ostream *s)
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
        FFTBuf<Ipp32f> *f = l.front();

        if(f->is_written()) {
            std::stringstream ss;
            ss << "Already written, delete" << std::endl;
            debug(ss.str());
            l.pop_front();
            delete f;
            continue;
        }

        f->wait_until_processed();

        if(!f->set_written())
        {
            continue;
        }
        {
            boost::mutex::scoped_lock lock(f->get_mutex());
            s->write((char *)f->cdata(), sizeof(*(f->cdata())) * (f->get_siglen() / 2));
        }
        std::stringstream ss;
        ss << "Done writing, deleting" << std::endl;
        debug(ss.str());
        l.pop_front();
        delete f;
    }
    std::stringstream ss;
    ss << "Quit" << std::endl;
    debug(ss.str());
}
