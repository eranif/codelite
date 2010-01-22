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

#include <wx/dcmemory.h>
#include <wx/xrc/xmlres.h>
#include "aboutdlg.h"
#include "contributers.h"
#include "windowattrmanager.h"

AboutDlg::AboutDlg( wxWindow* parent, const wxString &mainTitle )
		: AboutDlgBase( parent )
{
	m_bmp = wxXmlResource::Get()->LoadBitmap(wxT("About"));
	m_bitmap->SetBitmap(m_bmp);

	// set the page content
	m_htmlWin3->SetPage(wxString::FromUTF8(about_hex));
	m_buttonOk->SetFocus();
	GetSizer()->Fit(this);

	WindowAttrManager::Load(this, wxT("AboutDialog"), NULL);
}

AboutDlg::~AboutDlg()
{
	WindowAttrManager::Save(this, wxT("AboutDialog"), NULL);
}


void AboutDlg::SetInfo(const wxString& info)
{
	m_staticTextInformation->SetLabel(info);
}

wxString AboutDlg::GetInfo() const
{
	return m_staticTextInformation->GetLabelText();
}
