#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>
/**
 * helper string methods
 */

/**
 * @brief return the string before the first occurance of 'c'
 * @param s input string
 * @param c delimiter char
 * @return string or the entire 's' string of 'c' is not found
 */
std::string before_first(const std::string &s, char c);

/**
 * @brief return string after the first occurance of 'c'
 * @param s input string
 * @param c delimiter char
 * @return string or empty string if 'c' is not found
 */
std::string after_first(const std::string &s, char c);

/**
 * @brief return true if s starts with prefix, false otherwise
 */
bool starts_with(const std::string &s, const std::string &prefix);

/**
 * @brief return true if s starts with prefix, false otherwise
 */
bool ends_with(const std::string &s, const std::string &suffix, std::string &rest);

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

/**
 * @brief Gets all characters before the last occurrence of c. Returns the empty string if c is not found
 */
std::string before_last(const std::string &str, char c);

/**
 * @brief Gets all the characters after the last occurrence of c. Returns the whole string if c is not found.
 */
std::string after_last(const std::string &str, char c);

#endif // __UTILS_H__
