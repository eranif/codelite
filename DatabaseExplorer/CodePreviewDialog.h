//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CodePreviewDialog.h
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

#ifndef CODEPREVIEWDIALOG_H
#define CODEPREVIEWDIALOG_H

#include <wx/wx.h>
#include "GUI.h" // Base class: _CodePreviewDialog
#include "DbViewerPanel.h"
/*! \brief Dialog for code preview. Dialog support syntax highlighting. */
class CodePreviewDialog : public _CodePreviewDialog {

public:
	CodePreviewDialog(wxWindow* parent, const wxString& sourceCode);
	virtual ~CodePreviewDialog();

public:
	virtual void OnOKClick(wxCommandEvent& event);
};

#endif // CODEPREVIEWDIALOG_H
