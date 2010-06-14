#ifndef __SPECTRA2_DEBUG_HPP_
#define __SPECTRA2_DEBUG_HPP_
#include <string>
#include <sstream>

void debug(std::string str);
void debug(std::stringstream &ss);
void debug(char *str);
void debug(int num);

void error(std::stringstream &ss);
void warning(std::stringstream &ss);
void notice(std::stringstream &ss);

#endif
