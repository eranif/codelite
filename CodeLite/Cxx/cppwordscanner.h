//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cppwordscanner.h
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
#ifndef __cppwordscanner__
#define __cppwordscanner__

#include "codelite_exports.h"
#include "cpptoken.h"
#include "macros.h"

#include <wx/string.h>

struct ByteState {
    short state;   // Holds the current byte state (one of CppWordScanner::STATE_*)
    short depth;   // The current char depth
    short depthId; // The depth ID (there can be multiple blocks of the same depth in a given scope)
    int lineNo;    // the line number which holds this byte
};

class WXDLLIMPEXP_CL CppWordScanner
{
    wxStringSet_t m_keywords;
    wxString m_filename;
    wxString m_text;
    int m_offset;

public:
    enum {
        STATE_NORMAL = 0,
        STATE_C_COMMENT,
        STATE_CPP_COMMENT,
        STATE_DQ_STRING,
        STATE_SINGLE_STRING,
        STATE_PRE_PROCESSING
    };

protected:
    void doFind(const wxString& filter, CppTokensMap& l, int from, int to);
    void doInit();

public:
    CppWordScanner() = default;
    CppWordScanner(const wxString& file_name);
    CppWordScanner(const wxString& file_name, const wxString& text, int offset);

    ~CppWordScanner() = default;

    void FindAll(CppTokensMap& l);
};

#endif // __cppwordscanner__
