#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "clist.h"

	typedef struct string_pair {
		char *key;
		char *data;
	} string_pair_t;

	/**
	 * \brief perform a find & replace on src string
	 * \param src source string
	 * \param find_what find what in the string
	 * \param replace_with replacement string
	 * \return string allocated using maclloc - Must be freed by the caller
	 */
	extern char* string_replace(const char* src, const char *find_what, const char *replace_with);

	/**
	 * \brief accepts a keyDELIMvalue and return a list
	 * \param src source string
	 * \param delim delimiter
	 * \return list of key/value pairs
	 */
	extern list_t* string_split(const char* src, const char *delim);

#ifdef __cplusplus
}
#endif


#endif /*__STRING_UTIL_H__*/
