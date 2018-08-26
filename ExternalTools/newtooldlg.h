//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newtooldlg.h
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

#ifndef __newtooldlg__
#define __newtooldlg__

#include "externaltooldlg.h"
#include "imanager.h"
#include "NewToolBase.h"

class NewToolDlg : public NewToolBase
{
    IManager* m_mgr;

protected:
    virtual void OnButtonOkUI(wxUpdateUIEvent& event);
    virtual void OnIdSelected(wxCommandEvent& event);
    // Handlers for NewToolBase events.
    void OnButtonBrowsePath(wxCommandEvent& event);
    void OnButtonBrowseWD(wxCommandEvent& event);
    void OnButtonHelp(wxCommandEvent& event);
    void OnButtonOk(wxCommandEvent& event);
    void OnButtonCancel(wxCommandEvent& event);
    void OnButtonBrowseIcon16(wxCommandEvent& event);
    void OnButtonBrowseIcon24(wxCommandEvent& event);

public:
    /** Constructor */
    NewToolDlg(wxWindow* parent, IManager* mgr, ExternalToolData* data);
    virtual ~NewToolDlg();

    wxString GetPath() const { return m_textCtrlPath->GetValue(); }
    wxString GetWorkingDirectory() const { return m_textCtrlWd->GetValue(); }
    wxString GetToolId() const { return m_choiceId->GetStringSelection(); }
    wxString GetToolName() const { return m_textCtrlName->GetValue(); }
    wxString GetIcon16() const { return m_textCtrlIcon16->GetValue(); }
    wxString GetIcon24() const { return m_textCtrlIcon24->GetValue(); }
    bool GetCaptureOutput() const { return m_checkBoxCaptureOutput->IsChecked(); }
    bool GetSaveAllFiles() const { return m_checkBoxSaveAllFilesBefore->IsChecked(); }
    bool IsCallOnFileSave() const { return m_checkBoxCallOnFileSave->IsChecked(); }
};

#endif // __newtooldlg__
