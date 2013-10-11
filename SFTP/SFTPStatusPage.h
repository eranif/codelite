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

class SFTPThreadMessage;
class SFTPStatusPage : public SFTPStatusPageBase
{
    SFTPImages m_bitmaps;
public:
    SFTPStatusPage(wxWindow* parent);
    virtual ~SFTPStatusPage();
    
    void AddLine( SFTPThreadMessage* message );
    void FileDownloadedSuccessfully( const wxString &localFileName );
    void ShowContextMenu();
    
protected:
    virtual void OnContentMenu(wxDataViewEvent& event);
    virtual void OnClearLog(wxCommandEvent& event);
    
};
#endif // SFTPSTATUSPAGE_H
