#include "output.hpp"
#include <boost/thread/mutex.hpp>

void output(List<FFTBuf<Ipp16s> *> &l, std::ostream &s)
{
    while(true)
    {
        while(l.empty())
        {
            std::cerr << "Before empty wait" << std::endl;
            l.wait();
            std::cerr << "After empty wait" << std::endl;
        }
        FFTBuf<Ipp16s> *f = l.front();

        if(f->is_written()) {
            delete f;
            l.pop_front();
            continue;
        }

        f->wait_until_processed();

        if(!f->set_written())
        {
            continue;
        }
        {
            boost::mutex::scoped_lock lock(f->get_mutex());
            s.write((char *)f->cdata(), sizeof(*(f->cdata())) * f->get_siglen());
        }
        delete f;
        l.pop_front();
    }
    std::cerr << "Quit" << std::endl;
}
