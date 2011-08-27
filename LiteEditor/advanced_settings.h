//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : advanced_settings.h
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
 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __advanced_settings__
#define __advanced_settings__

#include <wx/wx.h>

#include <wx/button.h>
#include "compiler.h"
#include <wx/statline.h>
#include <wx/treebook.h>
#include <wx/panel.h>
#include "build_page.h"
#include <wx/toolbook.h>
#include <map>
#include <vector>

///////////////////////////////////////////////////////////////////////////
class BuildTabSetting;

///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedDlg
///////////////////////////////////////////////////////////////////////////////

class ICompilerSubPage {
public:
	ICompilerSubPage(){}
	virtual ~ICompilerSubPage(){}
	virtual void Save(CompilerPtr cmp) = 0;
};

class AdvancedDlg : public wxDialog
{
	DECLARE_EVENT_TABLE();
	std::map<wxString, std::vector<ICompilerSubPage*> > m_compilerPagesMap;

protected:
	wxNotebook* m_notebook;
	wxPanel* m_compilersPage;
	wxStaticText* m_staticText1;
	wxButton* m_buttonNewCompiler;
	wxStaticLine* m_staticline2;
	wxTreebook* m_compilersNotebook;
	wxStaticLine* m_staticline10;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	wxButton* m_buttonRestoreDefaults;
	BuildPage *m_buildPage;
	BuildTabSetting *m_buildSettings;
	wxMenu *m_rightclickMenu;

	void OnButtonNewClicked(wxCommandEvent &);
	void OnButtonOKClicked(wxCommandEvent &);
	void OnRestoreDefaults(wxCommandEvent &);
	void OnDeleteCompiler(wxCommandEvent &);
	void OnContextMenu(wxContextMenuEvent &e);
    
    void LoadCompilers();
	bool CreateDefaultNewCompiler(const wxString &name);
	void AddCompiler(CompilerPtr cmp, bool selected);
	bool DeleteCompiler(const wxString &name);
	void SaveCompilers();

public:
	AdvancedDlg( wxWindow* parent, size_t selected_page, int id = wxID_ANY, wxString title = _("Build Settings"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize(800, 700), int style = wxDEFAULT_DIALOG_STYLE );
	~AdvancedDlg();
};

#endif //__advanced_settings__
