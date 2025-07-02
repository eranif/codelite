//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_defs.h
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

#ifndef CL_DEFS_H
#define CL_DEFS_H

//-----------------------------------
// Use new toolbars where possible
//-----------------------------------
#include <wx/version.h>

#if defined(__WXMSW__) || defined(__WXMAC__)
#define MAINBOOK_AUIBOOK 0
#else
#define MAINBOOK_AUIBOOK 0
#endif

// Defaults
#ifdef __WXGTK__

//-------------------
// Linux macros
//-------------------

#ifndef PLUGINS_DIR
#define PLUGINS_DIR "/usr/lib/codelite"
#endif

#ifndef INSTALL_DIR
#define INSTALL_DIR "/usr/share/codelite"
#endif

#define CL_USE_NATIVEBOOK 0

#elif defined(__WXMSW__)

//-------------------
// Windows
//-------------------

#ifdef USE_POSIX_LAYOUT
#ifndef PLUGINS_DIR
#define PLUGINS_DIR "\\lib\\codelite"
#endif
#ifndef INSTALL_DIR
#define INSTALL_DIR "\\share\\codelite"
#endif
#endif

#define CL_USE_NATIVEBOOK 0
#else

//-------------------
// macOS
//-------------------
#define CL_USE_NATIVEBOOK 0

#endif

// General macros
#define CL_USE_NEW_BUILD_TAB 1
#define CL_N0_OF_BOOKMARK_TYPES 5

#endif // CL_DEFS_H
