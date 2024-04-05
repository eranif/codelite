//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
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

#include "cl_command_event.h"

#include <memory>
#include <vector>
#include <wx/arrstr.h>
#include <wx/event.h> // Base class: wxEvtHandler

class WebTools;
class IEditor;
class CSSCodeCompletion : public wxEvtHandler
{
public:
    using Ptr_t = std::unique_ptr<CSSCodeCompletion>;
    struct Entry {
        wxString property;
        wxArrayString values;
        typedef std::vector<Entry> Vec_t;
    };

protected:
    bool m_isEnabled;
    Entry::Vec_t m_entries;
    WebTools* m_plugin = nullptr;

protected:
    wxString GetPreviousWord(IEditor* editor, int pos);
    void OnCodeComplete(clCodeCompletionEvent& event);
    IEditor* GetEditor(const wxString& filename) const;

public:
    CSSCodeCompletion(WebTools* plugin);
    virtual ~CSSCodeCompletion();

    void CssCodeComplete(IEditor* editor);
};

#endif // CSSCODECOMPLETION_H
