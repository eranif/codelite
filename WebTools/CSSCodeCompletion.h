//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
// File name            : CSSCodeCompletion.h
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

#ifndef CSSCODECOMPLETION_H
#define CSSCODECOMPLETION_H

#include <wx/event.h> // Base class: wxEvtHandler
#include "smart_ptr.h"
#include <vector>
#include <wx/arrstr.h>

class IEditor;
class CSSCodeCompletion : public wxEvtHandler
{

public:
    typedef SmartPtr<CSSCodeCompletion> Ptr_t;
    struct Entry {
        wxString property;
        wxArrayString values;
        typedef std::vector<Entry> Vec_t;
    };

protected:
    bool m_isEnabled;
    Entry::Vec_t m_entries;

protected:
    wxString GetPreviousWord(IEditor* editor, int pos);

public:
    CSSCodeCompletion();
    virtual ~CSSCodeCompletion();

    void CssCodeComplete(IEditor* editor);
};

#endif // CSSCODECOMPLETION_H
