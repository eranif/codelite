//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clStringHistory.h
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

#ifndef CLSTRINGHISTORY_H
#define CLSTRINGHISTORY_H
#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clStringHistory
{
    wxArrayString m_strings;
    int m_index;

public:
    clStringHistory();
    virtual ~clStringHistory();

    /**
     * @brief can we move to the next item in the history?
     */
    bool CanNext();
    /**
     * @brief can we move to the previous item in the history?
     */
    bool CanPrev();

    /**
     * @brief initialize the history strings
     */
    void SetStrings(const wxArrayString& strings)
    {
        this->m_strings = strings;
        m_index = 0;
    }

    /**
     * @brief return the current item
     */
    bool Current(wxString& str);
    /**
     * @brief return the next item in the history
     * @param str [output]
     * @return true on success, false otherwise
     */
    bool Next(wxString& str);
    /**
     * @brief return the previous item in the history
     * @param str [output]
     * @return true on success, false otherwise
     */
    bool Previous(wxString& str);
};

#endif // CLSTRINGHISTORY_H
