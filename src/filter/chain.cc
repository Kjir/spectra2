#include <boost/thread/mutex.hpp>
#include "data_length.hpp"
#include "filter/chain.hpp"
#include "fft_buf.hpp"
#include "ipp.hpp"

FilterChain::execute(SrcType<IppType> &src, FFTBufPtr dst)
{
    DstIppType *tmpdst = IPP::alloc(tmpdst, dst->get_siglen());
    //foreach
    ProcessFilter f = _filters.front();
    f.transform(src.data, tmpdst);
    //endforeach
    {
        boost::mutex::scoped_lock lock(data.get_mutex());
        _merge.merge(src, dst, tmpdst);
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
