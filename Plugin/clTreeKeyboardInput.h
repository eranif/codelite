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

#include "codelite_exports.h"
#include "wx/event.h"
#include <wx/sharedptr.h>
#include <wx/treebase.h>
#include <list>
#include <vector>

class wxTextCtrl;
class wxTreeCtrl;
class WXDLLIMPEXP_SDK clTreeKeyboardInput : public wxEvtHandler
{

public:
    typedef wxSharedPtr<clTreeKeyboardInput> Ptr_t;

protected:
    wxTreeCtrl* m_tree;
    wxTextCtrl* m_text;
    std::list<wxTreeItemId> m_items;

protected:
    void OnKeyDown(wxKeyEvent& event);
    void OnTextKeyDown(wxKeyEvent& event);
    void OnTextUpdated(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnTreeFocus(wxFocusEvent& event);
    void OnTreeSize(wxSizeEvent& event);
    void SelecteItem(const wxTreeItemId& item);
    bool CheckItemForMatch(const wxTreeItemId& item);
    void SetTextFocus();
    void GetChildren(const wxTreeItemId& from = wxTreeItemId(), const wxTreeItemId& until = wxTreeItemId());
    void DoGetChildren(const wxTreeItemId& parent);
    void Clear();
    void DoShowTextBox();
    void SimulateKeyDown(const wxKeyEvent& event);

public:
    clTreeKeyboardInput(wxTreeCtrl* tree);
    virtual ~clTreeKeyboardInput();
};

#endif // CLTREEKEYBOARDINPUT_H
