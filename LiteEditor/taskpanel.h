//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : taskpanel.h
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

#ifndef __taskpanel__
#define __taskpanel__

#include "findresultstab.h"

class wxToggleButton;

class TaskPanel : public FindResultsTab
{
private:
    wxArrayString m_extensions;
    wxButton* m_findWhat;
    wxChoice* m_scope;
    wxChoice* m_choiceEncoding;

protected:
    SearchData DoGetSearchData();
    void OnFindWhat(wxCommandEvent& e);
    void OnSearch(wxCommandEvent& e);
    void OnSearchUI(wxUpdateUIEvent& e);
    void OnRepeatOutput(wxCommandEvent& e);
    void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    void OnEncodingSelected(wxCommandEvent& e);
    void OnSearchStart(wxCommandEvent& e);
    DECLARE_EVENT_TABLE()

public:
    TaskPanel(wxWindow* parent, wxWindowID id, const wxString& name);
    ~TaskPanel();
};

#endif // __taskpanel__
