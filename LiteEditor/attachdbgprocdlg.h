//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : attachdbgprocdlg.h
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
#ifndef __attachdbgprocdlg__
#define __attachdbgprocdlg__

#include "attachdbgprocbasedlg.hpp"

class AttachDbgProcDlg : public AttachDbgProcBaseDlg
{
public:
    void RefreshProcessesList(wxString filter);

    AttachDbgProcDlg(wxWindow* parent);
    ~AttachDbgProcDlg() override;

    wxString GetProcessId() const;
    wxString GetExeName() const;
    wxString GetDebugger() const { return m_choiceDebugger->GetStringSelection(); }

protected:
    // events
    void OnBtnAttachUI(wxUpdateUIEvent& event) override;
    void OnFilter(wxCommandEvent& event) override;
    void OnRefresh(wxCommandEvent& event) override;
    void OnEnter(wxCommandEvent& event) override;
    void OnItemActivated(wxDataViewEvent& event) override;
};

#endif // __attachdbgprocdlg__
