#ifndef __SPECTRA2_CHAIN_HPP_
#define __SPECTRA2_CHAIN_HPP_

#include <list>
#include "data_length.hpp"
#include "filter/process.hpp"
#include "filter/merge.hpp"

class FilterChain {
    public:
        FilterChain(MergeFilter *merge) : _merge(merge) {};
        virtual ~FilterChain() {};
        void push_filter(ProcessFilter *flt) { _filters.push_back(flt); };
        void pop_filter() { _filters.pop_front(); };
        void clear_filters() { _filters.clear(); }
        void execute(const SrcType<IppType> &src, FFTBufPtr dst);
    private:
        std::list<ProcessFilter *> _filters;
        /*
         * This abstract class represents a merge strategy: how do I copy the
         * results coming from my filters to my original destination buffer?
         * The implementation of this class gives the answer.
         */
        MergeFilter *_merge;
};

#endif /* __SPECTRA2_CHAIN_HPP_ */
