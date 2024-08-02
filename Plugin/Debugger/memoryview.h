//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : memoryview.h
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

#ifndef __memoryview__
#define __memoryview__

#include "memoryviewbase.h"

#include <codelite_exports.h>

/** Implementing MemoryViewBase */
class WXDLLIMPEXP_SDK MemoryView : public MemoryViewBase
{
protected:
    virtual void OnTextDClick(wxMouseEvent& event);
    virtual void OnTextEntered(wxCommandEvent& event);
    virtual void OnMemorySize(wxCommandEvent& event);
    virtual void OnNumberOfRows(wxCommandEvent& event);
    // Handlers for MemoryViewBase events.
    void OnEvaluate(wxCommandEvent& event);
    void OnEvaluateUI(wxUpdateUIEvent& event);
    void OnUpdate(wxCommandEvent& e);
    void OnUpdateUI(wxUpdateUIEvent& event);

    void UpdateDebuggerPane();

public:
    /** Constructor */
    MemoryView(wxWindow* parent);
    void Clear();
    wxString GetExpression() const { return m_textCtrlExpression->GetValue(); }
    size_t GetSize() const;
    size_t GetColumns() const;
    void SetViewString(const wxString& text);
};

#endif // __memoryview__
