#ifndef __SPECTRA2_DEBUG_HPP_
#define __SPECTRA2_DEBUG_HPP_
#include <string>
#include <sstream>

/**
 * @file
 * @author Stéphane Bisinger <stephane.bisinger@gmail.com>
 * 
 * @section DESCRIPTION
 * Debug functions to print debug messages, warnings, notices and errors.
 *
 */

/**
 * Print debug information, if enabled
 *
 * @param str The string to be printed
 */
void debug(std::string str);
/**
 * Print debug information, if enabled
 *
 * @param ss A stringstream containing the string to be printed
 */
void debug(std::stringstream &ss);
/**
 * Print debug information, if enabled
 *
 * @param str The string to be printed
 */
void debug(char *str);
/**
 * Print debug information, if enabled
 *
 * @param num An int to be printed
 */
void debug(int num);

/**
 * Print error messages
 *
 * @param ss A stringstream containing the error message
 */
void error(std::stringstream &ss);
/**
 * Print a warning
 *
 * @param ss A stringstream with the warning to be printed
 */
void warning(std::stringstream &ss);
/**
 * Print a notice
 *
 * @param ss A stringstream with the notice string to be printed
 */
void notice(std::stringstream &ss);

#endif
