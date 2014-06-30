//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : compat_win32.cpp
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
 * @file compat_win32.cpp
 * Compatibility functions for win32
 *
 * @author  Ben Gardner
 * @license GPL v2+
 */
#ifdef WIN32

#include "windows_compat.h"
#include "windows.h"
#include <string>
#include <cstdio>

bool unc_getenv(const char *name, std::string& str)
{
   DWORD len = GetEnvironmentVariableA(name, NULL, 0);
   char  *buf;

   if (len == 0)
   {
      if (GetLastError() == ERROR_ENVVAR_NOT_FOUND)
      {
         return false;
      }
   }

   buf = (char *)malloc(len);
   if (buf)
   {
      len = GetEnvironmentVariableA(name, buf, len);
   }
   buf[len] = 0;

   str = buf;
   printf("%s: name=%s len=%d value=%s\n", __func__, name, (int)len, str.c_str());
   free(buf);

   return true;
}

bool unc_homedir(std::string& home)
{
   if (unc_getenv("HOME", home))
   {
      return true;
   }
   if (unc_getenv("USERPROFILE", home))
   {
      return true;
   }
   std::string hd, hp;
   if (unc_getenv("HOMEDRIVE", hd) && unc_getenv("HOMEPATH", hp))
   {
      home = hd + hp;
      return true;
   }
   return false;
}

#endif /* ifdef WIN32 */
