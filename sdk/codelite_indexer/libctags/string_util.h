#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "clist.h"

	/**
	 * @brief perform a find & replace on src string
	 * @param src source string
	 * @param find_what find what in the string
	 * @param replace_with replacement string
	 * @return string allocated using maclloc - Must be freed by the caller
	 */
	extern char* string_replace(const char* src, const char *find_what, const char *replace_with);

#ifdef __cplusplus
}
#endif

#endif /*__STRING_UTIL_H__*/
