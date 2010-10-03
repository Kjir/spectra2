#include <boost/thread/mutex.hpp>
#include <list>
#include "data_length.hpp"
#include "filter/chain.hpp"
#include "fft_buf.hpp"
#include "ipp.hpp"

void FilterChain::execute(const SrcType<IppType> &src, FFTBufPtr dst)
{
    DstIppType *tmpdst = IPP::alloc(tmpdst, dst->get_siglen());
    //foreach
    std::list<ProcessFilter *>::iterator f;
    for(f = _filters.begin(); f != _filters.end(); f++) {
        (*f)->transform(src.data, tmpdst);
    }
    //endforeach
    {
        _merge->merge(src, dst, tmpdst);
    }
    IPP::free(src.data);
    {
        boost::mutex::scoped_lock lock(*src.mutex);
        src.erasable = true;
    }
    dst->inc_processed();
    IPP::free(tmpdst);
    tmpdst = NULL;
}
