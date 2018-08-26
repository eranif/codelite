//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clTabTogglerHelper.h
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

#ifndef CLTABTOGGLERHELPER_H
#define CLTABTOGGLERHELPER_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/event.h>
#include <wx/window.h>
#include "cl_command_event.h"
#include <wx/bitmap.h>
#include <wx/sharedptr.h>
#include "Notebook.h"

class WXDLLIMPEXP_SDK clTabTogglerHelper : public wxEvtHandler
{
    wxString m_outputTabName;
    wxWindow* m_outputTab;
    wxString m_workspaceTabName;
    wxWindow* m_workspaceTab;
    wxBitmap m_outputTabBmp;
    wxBitmap m_workspaceTabBmp;

protected:
    void OnToggleOutputTab(clCommandEvent& event);
    void OnToggleWorkspaceTab(clCommandEvent& event);
    
public:
    /**
     * @brief return true if "tabname" is in the notebook (does not matter if it is focused or not)
     * @return wxNOT_FOUND if the tab does not exists, otherwise, return the tab index
     */
    static int IsTabInNotebook(Notebook* book, const wxString& tabname);

public:
    typedef wxSharedPtr<clTabTogglerHelper> Ptr_t;

public:
    clTabTogglerHelper(
        const wxString& outputTabName, wxWindow* outputTab, const wxString& workspaceTabName, wxWindow* workspaceTab);
    virtual ~clTabTogglerHelper();

    clTabTogglerHelper& SetOutputTab(wxWindow* outputTab)
    {
        this->m_outputTab = outputTab;
        return *this;
    }
    clTabTogglerHelper& SetOutputTabBmp(const wxBitmap& outputTabBmp)
    {
        this->m_outputTabBmp = outputTabBmp;
        return *this;
    }
    clTabTogglerHelper& SetOutputTabName(const wxString& outputTabName)
    {
        this->m_outputTabName = outputTabName;
        return *this;
    }
    clTabTogglerHelper& SetWorkspaceTab(wxWindow* workspaceTab)
    {
        this->m_workspaceTab = workspaceTab;
        return *this;
    }
    clTabTogglerHelper& SetWorkspaceTabBmp(const wxBitmap& workspaceTabBmp)
    {
        this->m_workspaceTabBmp = workspaceTabBmp;
        return *this;
    }
    clTabTogglerHelper& SetWorkspaceTabName(const wxString& workspaceTabName)
    {
        this->m_workspaceTabName = workspaceTabName;
        return *this;
    }

    wxWindow* GetOutputTab() { return m_outputTab; }
    const wxBitmap& GetOutputTabBmp() const { return m_outputTabBmp; }
    const wxString& GetOutputTabName() const { return m_outputTabName; }
    wxWindow* GetWorkspaceTab() { return m_workspaceTab; }
    const wxBitmap& GetWorkspaceTabBmp() const { return m_workspaceTabBmp; }
    const wxString& GetWorkspaceTabName() const { return m_workspaceTabName; }
};

#endif // CLTABTOGGLERHELPER_H
