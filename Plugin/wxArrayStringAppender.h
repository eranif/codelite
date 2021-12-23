//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : wxArrayStringAppender.h
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

#ifndef WXARRAYSTRINGAPPENDER_H
#define WXARRAYSTRINGAPPENDER_H

#include <wx/arrstr.h>

struct wxArrayStringAppender {
    wxArrayString& m_arr;
    wxString m_strToAppend;
    bool m_prepending;

    wxArrayStringAppender(wxArrayString& arr, const wxString& appendThis, bool prepending = false)
        : m_arr(arr)
        , m_strToAppend(appendThis)
        , m_prepending(prepending)
    {
    }

    void operator()(wxString& str)
    {
        if(m_prepending) {
            str.Prepend(m_strToAppend);
        } else {
            str.Append(m_strToAppend);
        }
    }
};

#endif // WXARRAYSTRINGAPPENDER_H
