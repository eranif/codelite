//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : unc_ctype.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * @file unc_ctype.h
 * The ctype function are only required to handle values 0-255 and EOF.
 * A char is sign-extended when cast to an int.
 * With some C libraries, these values cause a crash.
 * These wrappers will properly handle all char values.
 *
 * @author  Ben Gardner
 * @license GPL v2+
 */
#ifndef UNC_CTYPE_H_INCLUDED
#define UNC_CTYPE_H_INCLUDED

#include <cctype>

/**
 * Truncate anything except EOF (-1) to 0-255
 */
static inline int unc_fix_ctype(int ch)
{
   return((ch == -1) ? -1 : (ch & 0xff));
}


static inline int unc_isspace(int ch)
{
   return(isspace(unc_fix_ctype(ch)));
}


static inline int unc_isprint(int ch)
{
   return(isprint(unc_fix_ctype(ch)));
}


static inline int unc_isalpha(int ch)
{
   return(isalpha(unc_fix_ctype(ch)));
}


static inline int unc_isalnum(int ch)
{
   return(isalnum(unc_fix_ctype(ch)));
}


static inline int unc_toupper(int ch)
{
   return(toupper(unc_fix_ctype(ch)));
}


static inline int unc_tolower(int ch)
{
   return(tolower(unc_fix_ctype(ch)));
}


static inline int unc_isxdigit(int ch)
{
   return(isxdigit(unc_fix_ctype(ch)));
}


static inline int unc_isdigit(int ch)
{
   return(isdigit(unc_fix_ctype(ch)));
}


static inline int unc_isupper(int ch)
{
   return(isalpha(unc_fix_ctype(ch)) && (unc_toupper(unc_fix_ctype(ch)) == ch));
}


#endif /* UNC_CTYPE_H_INCLUDED */
