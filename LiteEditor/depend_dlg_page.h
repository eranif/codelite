//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : depend_dlg_page.h
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

#ifndef __depend_dlg_page__
#define __depend_dlg_page__

#include "buildorderbasepage.h"

/** Implementing DependenciesPage */
class DependenciesPage : public DependenciesPageBase
{
    wxString m_projectName;
    bool m_dirty;
    wxString m_currentSelection;

protected:
    // Handlers for DependenciesPage events.
    void OnConfigChanged( wxCommandEvent& event );
    void OnCheckListItemToggled( wxCommandEvent& event );
    void OnMoveUp( wxCommandEvent& event );
    void OnMoveDown( wxCommandEvent& event );
    void OnApplyButton(wxCommandEvent &event);
    void OnApplyButtonUI(wxUpdateUIEvent &event);

protected:
    void Init();
    void OnUpCommand(wxListBox *list);
    void OnDownCommand(wxListBox *list);
    void DoPopulateControl(const wxString &configuration);

public:
    /** Constructor */
    DependenciesPage( wxWindow* parent, const wxString &projectName );
    void Save();
};

#endif // __depend_dlg_page__
