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
#include "cc_include_files.h"
#include "cc_clang_page.h"
#include "cc_advance_page.h"
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
#include "globals.h"

//---------------------------------------------------------

TagsOptionsDlg::TagsOptionsDlg( wxWindow* parent, const TagsOptionsData& data)
	: TagsOptionsBaseDlg( parent )
	, m_data(data)
{
	m_colour_flags = m_data.GetCcColourFlags();
	
	m_treebook->AddPage(0, _("General"));
	m_treebook->AddSubPage(new CCDisplayAndBehvior(m_treebook, data), _("Display and behavior"), true);
	m_treebook->AddSubPage(new CCColourisePage(m_treebook, data), _("Colouring"), false);
	m_treebook->AddSubPage(new CCTriggeringPage(m_treebook, data), _("Triggering"), false);
	
	m_includeFilesPage = new CCIncludeFilesPage(m_treebook, data); 
	m_treebook->AddPage(m_includeFilesPage, _("Include Files"), false);
	
	m_advancedPage = new CCAdvancePage(m_treebook, data, this);
	m_treebook->AddPage(m_advancedPage, _("Advanced"), false);
	
	m_clangPage = new CCClangPage(m_treebook, data);
	m_treebook->AddPage(m_clangPage, _("clang"), false);
	
	MSWSetNativeTheme(m_treebook->GetTreeCtrl());
	
	Centre();
	GetSizer()->Fit(this);
	WindowAttrManager::Load(this, wxT("TagsOptionsDlgAttr"), NULL);
}

TagsOptionsDlg::~TagsOptionsDlg()
{
	WindowAttrManager::Save(this, wxT("TagsOptionsDlgAttr"), NULL);
}

void TagsOptionsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	CopyData();
	EndModal(wxID_OK);
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

void TagsOptionsDlg::Parse()
{
	// Prepate list of files to work on
	wxArrayString files       = wxStringTokenize(m_advancedPage->GetFiles(), wxT(" \t"), wxTOKEN_STRTOK);
	wxArrayString searchPaths = m_includeFilesPage->GetIncludePaths();

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
