#ifndef __SPECTRA2_LIST_HPP_
#define __SPECTRA2_LIST_HPP_
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

template<class T> class List {
    private:
        std::list<T> _dst;
        boost::mutex _mut;
        boost::condition_variable _empty;
    public:
        void pop_front();
        void push_back(const T &x);
        T &front();
        T &back();
        void wait();
        void notify_one() { _empty.notify_one(); }
        void notify_all() { _empty.notify_all(); }
        bool empty();
        bool item_needed();
};

template<class T> bool List<T>::empty() {
    boost::mutex::scoped_lock lock(_mut);
    return _dst.empty();
}

template<class T> void List<T>::pop_front() {
    boost::mutex::scoped_lock lock(_mut);
    _dst.pop_front();
}

template<class T> void List<T>::push_back(const T &x) {
    boost::mutex::scoped_lock lock(_mut);
    _dst.push_back(x);
    notify_one();
}

template<class T> T& List<T>::front() {
    boost::mutex::scoped_lock lock(_mut);
    return _dst.front();
}

template<class T> T& List<T>::back() {
    boost::mutex::scoped_lock lock(_mut);
    return _dst.back();
}

template<class T> void List<T>::wait() {
    boost::mutex::scoped_lock lock(_mut);
    return _empty.wait(lock);
}

template<class T> bool List<T>::item_needed() {
    boost::mutex::scoped_lock lock(_mut);
    return _dst.empty() || _dst.back()->is_src_full();
}

#endif
