#include "string_util.h"

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

char* string_replace(const char* src, const char* from, const char* to)
{
	/*
	  * Find out the lengths of the source string, text to replace, and
	  * the replacement text.
	  */

	size_t size = strlen(src) + 1;
	size_t fromlen = strlen(from);
	size_t tolen = strlen(to);
	int whole_word = 0;

	/*
	 * Allocate the first chunk with enough for the original string.
	 */
	char *value = (char*)malloc(size);
	/*
	 * We need to return 'value', so let's make a copy to mess around with.
	 */
	char *dst = value;

	/*
	 * Before we begin, let's see if malloc was successful.
	 */
	if ( value != NULL ) {
		/*
		 * Loop until no matches are found.
		 */

		for ( ;; ) {
			/*
			 * Try to find the search text.
			 */
			whole_word = 0;
			static const char word_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_";

			const char *match = strstr(src, from);
			if(match) {

				/* make sure there is a complete word */
				char ch_before = ' ';
				char ch_after = ' ';

				if(match > src) {
					ch_before = *(match-1);
				}

				if(src+strlen(src) >= match + fromlen){
					ch_after = *(match + fromlen);
				}

				whole_word = 1;
				if(strchr(word_chars, ch_before) || strchr(word_chars, ch_after)){
					whole_word = 0;
				}
			}

			if ( match != NULL && whole_word ) {
				/*
				 * Found search text at location 'match'. :)
				 * Find out how many characters to copy up to the 'match'.
				 */
				size_t count = match - src;

				/*
				 * We are going to realloc, and for that we will need a
				 * temporary pointer for safe usage.
				 */
				char *temp;

				/*
				 * Calculate the total size the string will be after the
				 * replacement is performed.
				 */
				size += tolen - fromlen;

				/*
				 * Attempt to realloc memory for the new size.
				 */
				temp = (char*)realloc(value, size);

				if ( temp == NULL ) {
					/*
					 * Attempt to realloc failed. Free the previously malloc'd
					 * memory and return with our tail between our legs. :(
					 */
					free(value);
					return NULL;
				}

				/*
				  * The call to realloc was successful. :) But we'll want to
				  * return 'value' eventually, so let's point it to the memory
				  * that we are now working with. And let's not forget to point
				  * to the right location in the destination as well.
				  */
				dst = temp + (dst - value);
				value = temp;

				/*
				  * Copy from the source to the point where we matched. Then
				  * move the source pointer ahead by the amount we copied. And
				  * move the destination pointer ahead by the same amount.
				  */
				memmove(dst, src, count);
				src += count;
				dst += count;

				/*
				  * Now copy in the replacement text 'to' at the position of
				  * the match. Adjust the source pointer by the text we replaced.
				  * Adjust the destination pointer by the amount of replacement
				  * text.
				  */

				memmove(dst, to, tolen);
				src += fromlen;
				dst += tolen;
			} else { /* No match found. */
				/*
				  * Copy any remaining part of the string. This includes the null
				  * termination character.
				  */

				strcpy(dst, src);
				break;
			}
		}
	}

	return value;
}
#if 0
list_t* string_split(const char* src, const char* delim)
{
	list_t *l;
	char *line = (char*)0;
	char *ptr = (char*)src;
	char *v;

	list_init(&l);

	/* read the string as line by line */
	line = strtok(ptr, "\n");
	while (line) {
		char *key, *value;
		v = strstr(line, delim);
		if (v) {
			value = (char*)malloc(strlen(v+1)+1);
			strcpy(value, v+1);

			key = (char*)malloc(v - line + 1);
			memcpy(key, line, v-line);
			key[v-line] = 0;

			/* allocate string pair and append it to the list */
			string_pair_t *pair = (string_pair_t*)malloc(sizeof(string_pair_t));
			pair->key = key;
			pair->data = value;
			list_append(l, (void*)pair);

		}
		line = strtok((char*)0, "\n");
	}
	return l;
}

#endif
