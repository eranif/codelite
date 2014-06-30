//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CodePreviewDialog.cpp
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

#include "CodePreviewDialog.h"

CodePreviewDialog::CodePreviewDialog(wxWindow* parent, const wxString& sourceCode):_CodePreviewDialog(parent)
{	
	m_scintilla3->SetText(sourceCode);
	m_scintilla3->SetReadOnly(true);
	DbViewerPanel::InitStyledTextCtrl( m_scintilla3 );
}

CodePreviewDialog::~CodePreviewDialog()
{
}

void CodePreviewDialog::OnOKClick(wxCommandEvent& event)
{
	Destroy();
}
