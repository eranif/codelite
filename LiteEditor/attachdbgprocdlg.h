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

#include "attachdbgprocbasedlg.h"

class AttachDbgProcDlg : public AttachDbgProcBaseDlg
{
public:
    void RefreshProcessesList(wxString filter);

    AttachDbgProcDlg(wxWindow* parent);
    virtual ~AttachDbgProcDlg();

    wxString GetProcessId() const;
    wxString GetExeName() const;
    wxString GetDebugger() const { return m_choiceDebugger->GetStringSelection(); }

protected:
    // events
    virtual void OnBtnAttachUI(wxUpdateUIEvent& event);
    virtual void OnFilter(wxCommandEvent& event);
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
};

#endif // __attachdbgprocdlg__
