#include "utils.h"

/**
 * helper string methods
 */

/**
 * @brief return the string before the first occurance of 'c'
 * @param s input string
 * @param c delimiter char
 * @return string or the entire 's' string of 'c' is not found
 */
std::string before_first(const std::string &s, char c)
{
	std::string::size_type where = s.find(c);
	if (where == std::string::npos) {
		return s;
	}
	return s.substr(0, where);
}

/**
 * @brief return string after the first occurance of 'c'
 * @param s input string
 * @param c delimiter char
 * @return string or empty string if 'c' is not found
 */
std::string after_first(const std::string &s, char c)
{
	std::string::size_type where = s.find(c);
	if (where == std::string::npos) {
		return "";
	}
	return s.substr(where);
}

/**
 * @brief return true if s starts with prefix, false otherwise
 */
bool starts_with(const std::string &s, const std::string &prefix)
{
	if (s.find(prefix) == 0) {
		return true;
	}
	return false;
}

/**
 * @brief return true if s starts with prefix, false otherwise
 */
bool ends_with(const std::string &s, const std::string &suffix, std::string &rest)
{
	std::string::size_type where = s.rfind(suffix);
	if (where != std::string::npos && ((s.length() - where) == suffix.length()) ) {
		rest = s.substr(where);
		return true;
	}
	return false;
}

/**
 * @brief remove whitespaces from string
 * @param str
 */
void string_trim(std::string &str)
{
	str.erase(0, str.find_first_not_of(" \t\n\r\v"));
	str.erase(str.find_last_not_of(" \t\n\r\v")+1);
}

/**
 * @brief tokenize string into array of string by delimiters
 * @param str input string
 * @param delimiters delimiters to break the string according to
 * @return vector of strings
 */
std::vector<std::string> string_tokenize(const std::string &str, const std::string& delimiter)
{
	std::string::size_type start (0);
	std::vector<std::string> tokens;
	std::string token;

	std::string::size_type end = str.find(delimiter);
	while ( end != std::string::npos ) {

		if ( end != start )
			token = str.substr(start, end - start);
		else
			token.clear();

		// trim spaces
		string_trim(token);

		if ( !token.empty() )
			tokens.push_back(token);

		// next token
		start = end + delimiter.length();
		end = str.find(delimiter, start );
	}

	if ( start != (str.length() - 1) ) {
		// We have another token which is not delimited
		token = str.substr(start);
		tokens.push_back(token);
	}
	return tokens;
}

/**
 * @brief Gets all characters before the last occurrence of c. Returns the empty string if c is not found
 */
std::string before_last(const std::string &str, char c)
{
	std::string::size_type where = str.rfind(c);
	if (where == std::string::npos) {
		return "";
	}
	return str.substr(0, where);
}

/**
 * @brief Gets all the characters after the last occurrence of c. Returns the whole string if c is not found.
 */
std::string after_last(const std::string &str, char c)
{
	std::string::size_type where = str.rfind(c);
	if (where == std::string::npos) {
		return str;
	}
	return str.substr(where+1);
}
