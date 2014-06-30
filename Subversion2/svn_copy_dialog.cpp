//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : svn_copy_dialog.cpp
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

#include "svn_copy_dialog.h"
#include "SvnCommitDialog.h"

SvnCopyDialog::SvnCopyDialog( wxWindow* parent )
: SvnCopyDialogBase( parent )
{
	m_textCtrlTargetURL->SetFocus();
	m_textCtrlTargetURL->SelectAll();
	m_textCtrlTargetURL->SetInsertionPointEnd();
}

void SvnCopyDialog::SetSourceURL(const wxString& url)
{
	m_textCtrlSourceURL->SetValue(url);
}

void SvnCopyDialog::SetTargetURL(const wxString& url)
{
	m_textCtrlTargetURL->SetValue(url);
}

wxString SvnCopyDialog::GetMessage()
{
	return SvnCommitDialog::NormalizeMessage(m_textCtrlComment->GetValue());
}

wxString SvnCopyDialog::GetSourceURL()
{
	return m_textCtrlSourceURL->GetValue();
}

wxString SvnCopyDialog::GetTargetURL()
{
	return m_textCtrlTargetURL->GetValue();
}
