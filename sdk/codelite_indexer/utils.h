#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>

/**
 * @brief remove whitespaces from string
 * @param str
 */
void string_trim(std::string &str);

/**
 * @brief tokenize string into array of string by delimiters
 * @param str input string
 * @param delimiters delimiters to break the string according to
 * @return vector of strings
 */
std::vector<std::string> string_tokenize(const std::string &str, const std::string& delimiter);

//------------------------------------------------------------
// process methods
//------------------------------------------------------------
/**
 * @brief return true if PID is alive
 * @param pid process id
 * @return true if the process alive, false otherwise
 */
bool is_process_alive(long pid);

#endif // __UTILS_H__
