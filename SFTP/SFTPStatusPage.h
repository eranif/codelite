//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : SFTPStatusPage.h
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

#ifndef SFTPSTATUSPAGE_H
#define SFTPSTATUSPAGE_H

#include "UI.h"
#include <wx/menu.h>
#include "cl_command_event.h"
#include "SFTPGrepStyler.h"
#include <wx/stc/stc.h>

class SFTPThreadMessage;
class SFTP;

class SFTPStatusPage : public SFTPStatusPageBase
{
    SFTPImages m_bitmaps;
    SFTP* m_plugin;
    SFTPGrepStyler::Ptr_t m_styler;

public:
    SFTPStatusPage(wxWindow* parent, SFTP* plugin);
    virtual ~SFTPStatusPage();

    void AddLine(SFTPThreadMessage* message);
    void ShowContextMenu();
    void SetStatusBarMessage(const wxString& message);
    void ClearSearchOutput();
    void ShowSearchTab();
    void ShowLogTab();
    void AddSearchText(const wxString& text);
    
protected:
    virtual void OnContentMenu(wxContextMenuEvent& event);
    virtual void OnClearLog(wxCommandEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnSelectAll(wxCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);

    void OnFindOutput(clCommandEvent& event);
    void OnFindFinished(clCommandEvent& event);
    void OnFindError(clCommandEvent& event);
    void OnHotspotClicked(wxStyledTextEvent& event);
    int GetTabIndex(const wxString& name) const;
};
#endif // SFTPSTATUSPAGE_H
