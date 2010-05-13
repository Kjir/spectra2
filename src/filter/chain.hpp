#ifndef __SPECTRA2_CHAIN_HPP_
#define __SPECTRA2_CHAIN_HPP_

#include <list>

class FilterChain {
    public:
        virtual ~FilterChain() {};
        void set_src(SourceFilter *src) { _src = src; };
        void set_sink(SinkFilter *sink) { _sink = sink; };
        void push_filter(ProcFilter *flt) { _filters.push_back(flt); };
        void pop_filter() { _filters.pop_front(); };
        void clear_filters() { _filters.clear(); }
    private:
        SourceFilter *_src;
        SinkFilter *_sink;
        std::list<ProcFilter *> _filters;
}

#endif /* __SPECTRA2_CHAIN_HPP_ */
