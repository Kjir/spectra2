#include "debug.hpp"
#include <boost/thread/mutex.hpp>

boost::mutex _debug_mut;

void debug(std::string str) {
#ifdef DEBUG
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << str;
#endif
}

void debug(char *str) {
#ifdef DEBUG
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << str;
#endif
}

void debug(int num) {
#ifdef DEBUG
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << num;
#endif
}
