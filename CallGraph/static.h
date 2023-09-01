//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : static.h
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

/***************************************************************
 * Name:      static.h
 * Purpose:   Header to create static variables.
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef __CALLGRAPH_STATIC_H__
#define __CALLGRAPH_STATIC_H__

#include <wx/string.h>

extern const wxString GMON_FILENAME_OUT;
extern const wxString DOT_FILENAME_PNG;
extern const wxString DOT_FILENAME_TXT;
extern const wxString CALLGRAPH_DIR;

#ifdef __WXMSW__
extern const wxString GPROF_FILENAME_EXE;
extern const wxString DOT_FILENAME_EXE;
extern const wxString EXECUTABLE_EXTENSION;
#else
extern const wxString GPROF_FILENAME_EXE;
extern const wxString DOT_FILENAME_EXE;
extern const wxString EXECUTABLE_EXTENSION;
#endif

#endif // __CALLGRAPH_STATIC_H__
