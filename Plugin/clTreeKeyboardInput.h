//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clTreeKeyboardInput.h
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

#ifndef CLTREEKEYBOARDINPUT_H
#define CLTREEKEYBOARDINPUT_H

#include "clTreeCtrl.h"
#include "codelite_exports.h"
#include "wx/event.h"
#include <list>
#include <vector>
#include <wx/sharedptr.h>

class wxTextCtrl;
class WXDLLIMPEXP_SDK clTreeKeyboardInput : public wxEvtHandler
{

public:
    typedef wxSharedPtr<clTreeKeyboardInput> Ptr_t;

protected:
    clTreeCtrl* m_tree = nullptr;
    wxTextCtrl* m_text = nullptr;
    std::list<wxTreeItemId> m_items;

protected:
    void OnKeyDown(wxTreeEvent& event);
    void OnTextKeyDown(wxKeyEvent& event);
    void OnTextUpdated(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnTreeFocus(wxFocusEvent& event);
    void OnTreeSize(wxSizeEvent& event);
    void SelecteItem(const wxTreeItemId& item);
    void SetTextFocus();
    void Clear();
    void DoShowTextBox();
    void SimulateKeyDown(const wxKeyEvent& event);
    wxTreeItemId FindItem(const wxTreeItemId& from, const wxString& pattern, bool goingUp) const;

public:
    clTreeKeyboardInput(clTreeCtrl* tree);
    virtual ~clTreeKeyboardInput();
};

#endif // CLTREEKEYBOARDINPUT_H
