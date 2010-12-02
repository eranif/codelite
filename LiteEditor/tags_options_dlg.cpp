//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_dlg.cpp
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
// C++ code generated with wxFormBuilder (version Jun  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "cc_display_and_behavior_page.h"
#include "cc_colourise_page.h"
#include "cc_triggering_page.h"
#include <wx/tokenzr.h>
#include "pp_include.h"
#include "pluginmanager.h"
#include "pptable.h"
#include "ieditor.h"
#include "tags_options_dlg.h"
#include "ctags_manager.h"
#include "windowattrmanager.h"
#include "macros.h"
#include "wx/tokenzr.h"
#include "add_option_dialog.h"

//---------------------------------------------------------

TagsOptionsDlg::TagsOptionsDlg( wxWindow* parent, const TagsOptionsData& data)
	: TagsOptionsBaseDlg( parent )
	, m_data(data)
{
	m_colour_flags = m_data.GetCcColourFlags();
	InitValues();
	
	m_treebook->AddPage(new CCDisplayAndBehvior(m_treebook, data), _("Display and behvior"), true);
	m_treebook->AddPage(new CCColourisePage(m_treebook, data), _("Colouring"), false);
	m_treebook->AddPage(new CCTriggeringPage(m_treebook, data), _("Triggering"), false);
	
	WindowAttrManager::Load(this, wxT("TagsOptionsDlgAttr"), NULL);
}

TagsOptionsDlg::~TagsOptionsDlg()
{
	WindowAttrManager::Save(this, wxT("TagsOptionsDlgAttr"), NULL);
}

void TagsOptionsDlg::InitValues()
{
	//initialize the CodeLite page

	m_checkFilesWithoutExt->SetValue               (m_data.GetFlags() & CC_PARSE_EXT_LESS_FILES ? true : false);
	m_listBoxSearchPaths->Append                   (m_data.GetParserSearchPaths() );
	m_listBoxSearchPaths1->Append                  (m_data.GetParserExcludePaths() );
	m_textPrep->SetValue                           (m_data.GetTokens());
	m_textTypes->SetValue                          (m_data.GetTypes());
	m_textCtrlFilesList->SetValue                  (m_data.GetMacrosFiles());

	m_textFileSpec->SetValue(m_data.GetFileSpec());
	m_comboBoxLang->Clear();
	m_comboBoxLang->Append(m_data.GetLanguages());
	if ( m_data.GetLanguages().IsEmpty() == false ) {
		wxString lan = m_data.GetLanguages().Item(0);
		m_comboBoxLang->SetStringSelection(lan);

	} else {
		m_comboBoxLang->Append(wxT("c++"));
		m_comboBoxLang->SetSelection(0);

	}


}

void TagsOptionsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	CopyData();
	EndModal(wxID_OK);
}

void TagsOptionsDlg::OnButtonAdd(wxCommandEvent &event)
{
	wxUnusedVar(event);
	//pop up add option dialog
	AddOptionDlg dlg(this, m_textPrep->GetValue());
	if (dlg.ShowModal() == wxID_OK) {
		m_textPrep->SetValue(dlg.GetValue());
	}
}

void TagsOptionsDlg::CopyData()
{
	// Save data to the interal member m_data
	for(size_t i=0; i<m_treebook->GetPageCount(); i++) {
		TagsOptionsPageBase *page = dynamic_cast<TagsOptionsPageBase*>(m_treebook->GetPage(i));
		if(page) {
			page->Save(m_data);
		}
	}
	
	SetFlag(CC_PARSE_EXT_LESS_FILES,                m_checkFilesWithoutExt->IsChecked());
	m_data.SetFileSpec(m_textFileSpec->GetValue());

	m_data.SetTokens(m_textPrep->GetValue());
	m_data.SetTypes(m_textTypes->GetValue());
	m_data.SetLanguages(m_comboBoxLang->GetStrings());
	m_data.SetLanguageSelection(m_comboBoxLang->GetStringSelection());
	m_data.SetParserSearchPaths( m_listBoxSearchPaths->GetStrings() );
	m_data.SetParserExcludePaths( m_listBoxSearchPaths1->GetStrings() );
	m_data.SetMacrosFiles( m_textCtrlFilesList->GetValue() );

}

void TagsOptionsDlg::SetFlag(CodeCompletionOpts flag, bool set)
{
	if (set) {
		m_data.SetFlags(m_data.GetFlags() | flag);
	} else {
		m_data.SetFlags(m_data.GetFlags() & ~(flag));
	}
}

void TagsOptionsDlg::SetColouringFlag(CodeCompletionColourOpts flag, bool set)
{
	if (set) {
		m_data.SetCcColourFlags(m_data.GetCcColourFlags() | flag);
	} else {
		m_data.SetCcColourFlags(m_data.GetCcColourFlags() & ~(flag));
	}
}

void TagsOptionsDlg::OnAddSearchPath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxString new_path = wxDirSelector(wxT("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false) {
		if(m_listBoxSearchPaths->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxSearchPaths->Append(new_path);
		}
	}
}

void TagsOptionsDlg::OnAddSearchPathUI(wxUpdateUIEvent& e)
{
	e.Enable(true);
}

void TagsOptionsDlg::OnClearAll(wxCommandEvent& e)
{
	wxUnusedVar(e);
	m_listBoxSearchPaths->Clear();
}

void TagsOptionsDlg::OnClearAllUI(wxUpdateUIEvent& e)
{
	e.Enable(m_listBoxSearchPaths->IsEmpty() == false);
}

void TagsOptionsDlg::OnRemoveSearchPath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	int sel = m_listBoxSearchPaths->GetSelection();
	if( sel != wxNOT_FOUND) {
		m_listBoxSearchPaths->Delete((unsigned int)sel);
	}
}

void TagsOptionsDlg::OnRemoveSearchPathUI(wxUpdateUIEvent& e)
{
	e.Enable(m_listBoxSearchPaths->GetSelection() != wxNOT_FOUND);
}

void TagsOptionsDlg::OnAddExcludePath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxString new_path = wxDirSelector(wxT("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false) {
		if(m_listBoxSearchPaths1->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxSearchPaths1->Append(new_path);
		}
	}
}

void TagsOptionsDlg::OnAddExcludePathUI(wxUpdateUIEvent& e)
{
	e.Enable(true);
}

void TagsOptionsDlg::OnClearAllExcludePaths(wxCommandEvent& e)
{
	wxUnusedVar(e);
	m_listBoxSearchPaths1->Clear();
}

void TagsOptionsDlg::OnClearAllExcludePathsUI(wxUpdateUIEvent& e)
{
	e.Enable(m_listBoxSearchPaths1->IsEmpty() == false);
}

void TagsOptionsDlg::OnRemoveExcludePath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	int sel = m_listBoxSearchPaths1->GetSelection();
	if( sel != wxNOT_FOUND) {
		m_listBoxSearchPaths1->Delete((unsigned int)sel);
	}
}

void TagsOptionsDlg::OnRemoveExcludePathUI(wxUpdateUIEvent& e)
{
	e.Enable(m_listBoxSearchPaths1->GetSelection() != wxNOT_FOUND);
}

void TagsOptionsDlg::OnFileSelectedUI(wxUpdateUIEvent& event)
{
	event.Enable( m_textCtrlFilesList->GetValue().IsEmpty() == false);
}

void TagsOptionsDlg::OnParse(wxCommandEvent& event)
{
	// Prepate list of files to work on
	wxArrayString files = wxStringTokenize(m_textCtrlFilesList->GetValue(), wxT(" \t"), wxTOKEN_STRTOK);
	wxArrayString searchPaths = m_listBoxSearchPaths->GetStrings();

	wxArrayString fullpathsArr;

	for(size_t i=0; i<files.size(); i++) {
		wxString file = files[i].Trim().Trim(false);
		if(file.IsEmpty())
			continue;

		for(size_t xx=0; xx<searchPaths.size(); xx++) {
			wxString fullpath;
			fullpath << searchPaths.Item(xx) << wxFileName::GetPathSeparator() << file;
			wxFileName fn(fullpath);
			if(fn.FileExists()) {
				fullpathsArr.Add(fn.GetFullPath());
				break;
			}
		}
	}

	// Clear the PreProcessor table
	PPTable::Instance()->Clear();
	for(size_t i=0; i<fullpathsArr.size(); i++)
		PPScan( fullpathsArr.Item(i), true );

	// Open an editor and print out the results
	IEditor * editor = PluginManager::Get()->NewEditor();
	if(editor) {
		editor->AppendText( PPTable::Instance()->Export() );
		CopyData();
		EndModal(wxID_OK);
	}
}
