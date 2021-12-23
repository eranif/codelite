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

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wx/panel.h"
#include "wx/sizer.h"

#include <set>
#include <vector>
#include <wx/simplebook.h>

#define wxUSE_WINDOWSTACK_SIMPLEBOOK 0

#if !wxUSE_WINDOWSTACK_SIMPLEBOOK
class WXDLLIMPEXP_SDK WindowStack : public wxWindow
{
    std::vector<wxWindow*> m_windows;
    wxWindow* m_activeWin = nullptr;

protected:
    int FindPage(wxWindow* page) const;
    int ChangeSelection(size_t index);
    int DoSelect(wxWindow* win);
    void OnSize(wxSizeEvent& e);
    void DoHideNoActiveWindows();
    void OnColoursChanged(clCommandEvent& event);

public:
    WindowStack(wxWindow* parent, wxWindowID id = wxID_ANY, bool useNativeThemeColours = false);
    virtual ~WindowStack();

    bool Add(wxWindow* win, bool select);
    void Select(wxWindow* win);
    void Clear();

    bool Remove(wxWindow* win);

    bool Contains(wxWindow* win);
    bool IsEmpty() const { return m_windows.empty(); }
    wxWindow* GetSelected() const;
};
#else
class WXDLLIMPEXP_SDK WindowStack : public wxSimplebook
{
    int FindPage(wxWindow* win) const;
    void OnColoursChanged(clCommandEvent& event);

public:
    WindowStack(wxWindow* parent, wxWindowID id = wxID_ANY, bool useNativeThemeColours = false);
    virtual ~WindowStack();

    bool Add(wxWindow* win, bool select);
    void Select(wxWindow* win);
    void Clear();

    bool Remove(wxWindow* win);

    bool Contains(wxWindow* win);
    bool IsEmpty() const { return GetPageCount() == 0; }
    wxWindow* GetSelected() const;
};

#endif
#endif // WINDOWSTACK_H
