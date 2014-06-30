//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : wxcDownloadDlg.cpp
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

#include "wxcDownloadDlg.h"
#include <wx/utils.h>

wxcDownloadDlg::wxcDownloadDlg(wxWindow* parent)
    : wxcDownloadDlgBaseClass(parent)
{
    m_cmdLnkBtnDownload->SetDefault();
}

wxcDownloadDlg::~wxcDownloadDlg()
{
}
void wxcDownloadDlg::OnDownloadWxCrafterPlugin(wxCommandEvent& event)
{
    ::wxLaunchDefaultBrowser("http://wxcrafter.codelite.org");
    this->EndModal(wxID_OK);
}

void wxcDownloadDlg::OnIgnoreTheError(wxCommandEvent& event)
{
    this->EndModal(wxID_OK);
}
