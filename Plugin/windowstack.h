//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : windowstack.h
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
#ifndef WINDOWSTACK_H
#define WINDOWSTACK_H

#include "codelite_exports.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include <set>
#include <vector>

class WXDLLIMPEXP_SDK WindowStack : public wxPanel
{
    std::set<wxWindow*> m_windows;
    wxBoxSizer* m_mainSizer;
    wxWindow* m_selection;

    void DoSelect(wxWindow* win);

protected:
    void OnPaint(wxPaintEvent& evt);
    void OnEraseBG(wxEraseEvent& evt);

public:
    WindowStack(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~WindowStack();

    bool Add(wxWindow* win, bool select);
    void Select(wxWindow* win);
    void SelectNone();
    void Clear();

    bool Remove(wxWindow* win);
    bool Delete(wxWindow* win);

    bool Contains(wxWindow* win);

    int GetPageCount() const { return m_windows.size(); }
    bool IsEmpty() const { return GetPageCount() == 0; }
    wxWindow* GetSelected() { return m_selection; }
};

#endif // WINDOWSTACK_H
