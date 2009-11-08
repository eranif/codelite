//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : importfilesdialog.h
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

#ifndef __importfilesdialog__
#define __importfilesdialog__

#include <wx/intl.h>

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ImportFilesDialog
///////////////////////////////////////////////////////////////////////////////
class CheckDirTreeCtrl;
class ImportFilesDialog : public wxDialog
{
private:

protected:
	wxStaticText*     m_staticText2;
	wxButton*         m_button3;
	wxStaticLine*     m_staticline2;
	CheckDirTreeCtrl* m_treeCtrlDir;
	wxStaticText*     m_staticText1;
	wxTextCtrl*       m_textCtrSpec;
	wxCheckBox*       m_checkBoxFilesWOExt;
	wxStaticLine*     m_staticline1;
	wxButton*         m_buttonOk;
	wxButton*         m_buttonCancel;

protected:
	void OnChangeDir( wxCommandEvent& event );

public:
	ImportFilesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Import Files"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	virtual ~ImportFilesDialog();

	void     GetDirectories(wxArrayString &dirs);
	bool     ExtlessFiles();
	wxString GetFileMask();
	wxString GetBaseDir();
};

#endif //__importfilesdialog__
