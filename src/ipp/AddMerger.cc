#include <boost/thread/mutex.hpp>
#include "data_length.hpp"
#include "ipp/AddMerger.hpp"
#include "ipp.hpp"


void AddMerger::merge(const SrcType<IppType> &src, FFTBufPtr dst, DstIppType *result)
{
    boost::mutex::scoped_lock lock(dst->get_mutex());
    IPP::add(result, dst->cdata(), dst->get_siglen());
}
