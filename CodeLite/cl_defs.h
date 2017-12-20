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

#define clToolBar wxAuiToolBar
#ifdef __WXMSW__
#define clTB_DEFAULT_STYLE wxAUI_TB_PLAIN_BACKGROUND
#define clTB_DEFAULT_STYLE_PLUGIN wxAUI_TB_PLAIN_BACKGROUND
#else
#define clTB_DEFAULT_STYLE wxAUI_TB_DEFAULT_STYLE
#define clTB_DEFAULT_STYLE_PLUGIN wxAUI_TB_DEFAULT_STYLE
#endif
#define USE_AUI_TOOLBAR 1
#define USE_AUI_NOTEBOOK 0

// Defaults
#ifdef __WXGTK__
#ifndef PLUGINS_DIR
#define PLUGINS_DIR "/usr/lib/codelite"
#endif
#ifndef INSTALL_DIR
#define INSTALL_DIR "/usr/share/codelite"
#endif
#define CL_USE_NATIVEBOOK 0
#else
#ifdef USE_POSIX_LAYOUT
#ifndef PLUGINS_DIR
#define PLUGINS_DIR "\\lib\\codelite"
#endif
#ifndef INSTALL_DIR
#define INSTALL_DIR "\\share\\codelite"
#endif
#ifndef CFGDIR
#define CFGDIR "\\share\\codelite\\config\\cfg"
#endif
#endif
// MSW / OSX and the rest of the world
#define CL_USE_NATIVEBOOK 0
#endif

// Allow override the default CL_USE_NATIVEBOOK by cmake variable (GTK only)
#ifdef __WXGTK__
#if GTK_USE_NATIVEBOOK
#ifdef CL_USE_NATIVEBOOK
#undef CL_USE_NATIVEBOOK
#endif
#define CL_USE_NATIVEBOOK 1
#else
#ifdef CL_USE_NATIVEBOOK
#undef CL_USE_NATIVEBOOK
#endif
#define CL_USE_NATIVEBOOK 0
#endif
#endif

#if wxVERSION_NUMBER < 2904
#define CL_USE_NEW_BUILD_TAB 0
#else
#define CL_USE_NEW_BUILD_TAB 1
#endif

#define CL_N0_OF_BOOKMARK_TYPES 5

#endif // CL_DEFS_H
