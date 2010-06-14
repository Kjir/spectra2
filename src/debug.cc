#include "debug.hpp"
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <sstream>

boost::mutex _debug_mut;

void debug(std::string str) {
#ifdef DEBUG
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << str;
#endif
}

void debug(std::stringstream &ss) {
#ifdef DEBUG
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << ss.str();
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

void error(std::stringstream &ss) {
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << "ERROR: " << ss.str();
}

void warning(std::stringstream &ss) {
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << "Warning: " << ss.str();
}

void notice(std::stringstream &ss) {
    boost::mutex::scoped_lock lock(_debug_mut);
    std::cerr << "Notice: " << ss.str();
}
