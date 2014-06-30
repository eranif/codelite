//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : logmask.h
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
 * @file logmask.h
 *
 * Functions to manipulate a log severity mask.
 *
 * @author  Ben Gardner
 * @license GPL v2+
 */
#ifndef LOGMASK_H_INCLUDED
#define LOGMASK_H_INCLUDED

#include "base_types.h"
#include <cstring>     /* memset() */
#include <bitset>
#include "log_levels.h"


/** A simple array of 256 bits */
typedef std::bitset<256> log_mask_t;


/**
 * Tests whether a sev bit is set in the mask
 *
 * @param sev  The severity to check
 * @return     true (is set) or false (not set)
 */
static_inline bool logmask_test(const log_mask_t& mask, log_sev_t sev)
{
   return(mask.test(sev));
}


/**
 * Sets a set bit in the mask
 *
 * @param sev     The severity to check
 * @param value   true (set bit) or false (clear bit)
 */
static_inline void logmask_set_sev(log_mask_t& mask, log_sev_t sev, bool value)
{
   mask.set(sev, value);
}


/**
 * Sets all bits to the same value
 *
 * @param value   true (set bit) or false (clear bit)
 */
static_inline void logmask_set_all(log_mask_t& mask, bool value)
{
   if (value)
   {
      mask.set();
   }
   else
   {
      mask.reset();
   }
}


/**
 * Convert a logmask into a string.
 * The string is a comma-delimited list of severities.
 * Example: 1,3,5-10
 *
 * @param mask the mask to convert
 * @param buf  the buffer to hold the string
 * @param size the size of the buffer
 * @return     buf (pass through)
 */
char *logmask_to_str(const log_mask_t& mask, char *buf, int size);


/**
 * Parses a string into a log severity
 *
 * @param str     The string to parse
 * @param mask    The mask to populate
 */
void logmask_from_string(const char *str, log_mask_t& mask);


#endif /* LOGMASK_H_INCLUDED */
