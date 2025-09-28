//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : SelectDropTargetDlg.h
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

#ifndef SELECTDROPTARGETDLG_H
#define SELECTDROPTARGETDLG_H
#include "wxcrafter.h"

#include <map>

class SelectDropTargetDlg : public SelectDropTargetBaseDlg
{
    std::map<wxString, wxWindow*> m_views;
    wxArrayString m_folders;

public:
    SelectDropTargetDlg(wxWindow* parent, const wxArrayString& folders);
    virtual ~SelectDropTargetDlg();

protected:
    virtual void OnSelectionActivated(wxDataViewEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    void Initialize();
    void ActivateSelection();
};
#endif // SELECTDROPTARGETDLG_H
