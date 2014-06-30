//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : compat_posix.cpp
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
 * @file compat_posix.cpp
 * Compatibility functions for POSIX
 *
 * @author  Ben Gardner
 * @license GPL v2+
 */
#ifndef WIN32

#include <cstdlib>
#include <string>

bool unc_getenv(const char *name, std::string& str)
{
   const char *val = getenv(name);

   if (val)
   {
      str = val;
      return true;
   }
   return false;
}

bool unc_homedir(std::string& home)
{
   if (unc_getenv("HOME", home))
   {
      return true;
   }
   return false;
}

#endif /* ifndef WIN32 */
