//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : svn_patch_dlg.cpp
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

#include "svn_patch_dlg.h"
#include "windowattrmanager.h"
#include "editor_config.h"

PatchDlg::PatchDlg( wxWindow* parent )
	: PatchDlgBase( parent )
{
	WindowAttrManager::Load(this, wxT("PatchDlg"), NULL);
	
	long v;
	if(EditorConfigST::Get()->GetLongValue(wxT("m_radioBoxEOLPolicy"), v)) {
		m_radioBoxEOLPolicy->SetSelection(v);
	}
}

PatchDlg::~PatchDlg()
{
	WindowAttrManager::Save(this, wxT("PatchDlg"), NULL);
	EditorConfigST::Get()->SaveLongValue(wxT("m_radioBoxEOLPolicy"), m_radioBoxEOLPolicy->GetSelection());
}
