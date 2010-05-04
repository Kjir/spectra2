#include "output.hpp"
#include <boost/thread/mutex.hpp>

void output(List<FFTBuf<Ipp16s> *> &l, std::ostream *s)
{
    while(true)
    {
        while(l.empty())
        {
            std::cerr << "Before empty wait" << std::endl;
            s->flush();
            l.wait();
            std::cerr << "After empty wait" << std::endl;
        }
        FFTBuf<Ipp16s> *f = l.front();

        if(f->is_written()) {
            std::cerr << "Already written, delete" << std::endl;
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
            s->write((char *)f->cdata(), sizeof(*(f->cdata())) * f->get_siglen());
        }
        std::cerr << "Done writing, deleting" << std::endl;
        l.pop_front();
        delete f;
    }
    std::cerr << "Quit" << std::endl;
}
