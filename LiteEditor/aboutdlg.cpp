//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : aboutdlg.cpp
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
#include "precompiled_header.h"
#include <wx/ffile.h>
#include <wx/dcmemory.h>
#include <wx/xrc/xmlres.h>
#include <wx/settings.h>
#include "aboutdlg.h"
#include "contributers.h"
#include "windowattrmanager.h"
#include "manager.h"

AboutDlg::AboutDlg( wxWindow* parent, const wxString &mainTitle )
		: AboutDlgBase( parent )
{
	wxFileName splashscreen(ManagerST::Get()->GetStarupDirectory() +
							wxFileName::GetPathSeparator() + 
							wxT("images") + 
							wxFileName::GetPathSeparator() + 
							wxT("splashscreen.png"));

	m_bmp.LoadFile(splashscreen.GetFullPath(), wxBITMAP_TYPE_PNG);
	m_bitmap->SetBitmap(m_bmp);
	GetSizer()->Fit(this);
	
	// set the page content
	m_htmlWin3->SetPage(wxString::FromUTF8(about_hex));
	m_buttonOk->SetFocus();
	
	wxFileName license(ManagerST::Get()->GetStarupDirectory() + wxFileName::GetPathSeparator() + wxT("LICENSE"));
	if(license.FileExists()) {
		wxFFile fp(license.GetFullPath());
		if(fp.IsOpened()) {
			wxString content;
			fp.ReadAll(&content, wxConvUTF8);
			fp.Close();
			
			m_textCtrlLicense->SetEditable(true);
			wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
			font.SetFamily(wxFONTFAMILY_TELETYPE);
			m_textCtrlLicense->SetFont(font);
			
			m_textCtrlLicense->ChangeValue(content);
			m_textCtrlLicense->SetEditable(false);
		}
	}
	CentreOnScreen();
}

AboutDlg::~AboutDlg()
{
}

void AboutDlg::SetInfo(const wxString& info)
{
	m_staticTextInformation->SetLabel(info);
}

wxString AboutDlg::GetInfo() const
{
	return m_staticTextInformation->GetLabelText();
}
