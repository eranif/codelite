//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : VirtualDirectorySelectorDlg.h
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

#ifndef VIRTUALDIRECTORYSELECTORDLG_H
#define VIRTUALDIRECTORYSELECTORDLG_H

#include "VirtualDirectorySelectorBase.h"
#include "codelite_exports.h"
#include "clTreeKeyboardInput.h"

class clCxxWorkspace;

/** Implementing VirtualDirectorySelectorBase */
class WXDLLIMPEXP_SDK VirtualDirectorySelectorDlg : public VirtualDirectorySelectorDlgBaseClass
{
    clCxxWorkspace* m_workspace;
    wxString m_projectName;
    wxString m_initialPath;
    wxString m_suggestedName;
    wxImageList* m_images;
    bool m_reloadTreeNeeded;
    clTreeKeyboardInput::Ptr_t m_treeCtrlSearchHelper;

public:
    static wxString DoGetPath(wxTreeCtrl* tree, const wxTreeItemId& item, bool validateFolder);

protected:
    virtual void OnNewVDUI(wxUpdateUIEvent& event);
    virtual void OnNewVD(wxCommandEvent& event);
    void DoBuildTree();

protected:
    // Handlers for VirtualDirectorySelectorBase events.
    void OnItemSelected(wxTreeEvent& event);
    void OnButtonOK(wxCommandEvent& event);
    void OnButtonCancel(wxCommandEvent& event);
    void OnButtonOkUI(wxUpdateUIEvent& event);

public:
    /** Constructor */
    VirtualDirectorySelectorDlg(wxWindow* parent, clCxxWorkspace* wsp, const wxString& initialPath = wxEmptyString,
                                const wxString& projectname = wxEmptyString);
    ~VirtualDirectorySelectorDlg();
    wxString GetVirtualDirectoryPath() const { return m_staticTextPreview->GetLabel(); }
    bool SelectPath(const wxString& path);
    wxTreeItemId FindItemForPath(const wxString& path);
    void SetText(const wxString& text);
    void SetSuggestedName(const wxString& suggestedName) { m_suggestedName = suggestedName; }
};

#endif // VIRTUALDIRECTORYSELECTORDLG_H
