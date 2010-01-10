//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : open_type_dlg.cpp
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

#include "opentypevlistctrl.h"
#include "editor_config.h"
#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "open_type_dlg.h"
#include "globals.h"
#include "ctags_manager.h"
#include <wx/xrc/xmlres.h>
#include "macros.h"
#include "wx/imaglist.h"
#include "wx/timer.h"

static int OpenTypeDlgTimerId = wxNewId();

BEGIN_EVENT_TABLE(OpenTypeDlg, wxDialog)
	EVT_CHAR_HOOK(OpenTypeDlg::OnCharHook)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, OpenTypeDlg::OnItemActivated)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////

OpenTypeDlg::OpenTypeDlg( wxWindow* parent, TagsManager *tagsMgr, int id, wxString title, wxPoint pos, wxSize size, int style )
		: wxDialog( parent, id, title, pos, size, style )
		, m_selectedItem(wxNOT_FOUND)
{
	m_tagsManager = tagsMgr;
	
	Ctor();
}

OpenTypeDlg::OpenTypeDlg( wxWindow* parent, TagsManager *tagsMgr, const wxArrayString &kind, int id, wxString title, wxPoint pos, wxSize size, int style )
		: wxDialog( parent, id, title, pos, size, style )
		, m_selectedItem(wxNOT_FOUND)
		, m_kind(kind)
{
	m_tagsManager = tagsMgr;
	
	Ctor();	
}

OpenTypeDlg::~OpenTypeDlg()
{
	delete m_il;
}

void OpenTypeDlg::Ctor()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_staticText = new wxStaticText( this, wxID_ANY, wxT("Select a type to open (you may use wildcards):"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_staticText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

	m_textTypeName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_textTypeName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

	m_listTypes = new OpenTypeVListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL);
	mainSizer->Add( m_listTypes, 1, wxALL|wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonOK, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_button2, 0, wxALL, 5 );

	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );
	//assign image list to the dialog
	m_il = new wxImageList(16, 16, true);
	m_il->Add(wxXmlResource::Get()->LoadBitmap(_T("namespace")));
	m_il->Add(wxXmlResource::Get()->LoadBitmap(_T("class")));
	m_il->Add(wxXmlResource::Get()->LoadBitmap(_T("struct")));
	wxBitmap bmp;

	// typedef
	bmp = wxXmlResource::Get()->LoadBitmap(_T("typedef"));
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	m_il->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(_T("enum"));
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	m_il->Add(bmp);

	Init();
	
	ConnectButton(m_buttonOK, OpenTypeDlg::OnOK);
	ConnectCmdTextUpdated(m_textTypeName, OpenTypeDlg::OnText)
	this->SetSizer( mainSizer );
	this->Layout();
	Centre();
}

void OpenTypeDlg::Init()
{
	m_listTypes->InsertColumn(0, wxT("Symbol"));
	m_listTypes->InsertColumn(1, wxT("Scope"));
	m_listTypes->InsertColumn(2, wxT("File"));
	m_listTypes->InsertColumn(3, wxT("Line"));
	m_listTypes->SetColumnWidth(0, 200);
	
	std::vector<TagEntryPtr> tags;
	
	if (m_kind.IsEmpty())
	{
		m_tagsManager->OpenType(tags);
	}
	else
	{
		m_tagsManager->GetTagsByKind(tags, m_kind);
	}
	
	m_listTypes->SetImageList(m_il, wxIMAGE_LIST_SMALL);
	m_listTypes->SetItems(tags);
	m_listTypes->SetItemCount(tags.size());
	
	if(tags.size()>0){
		m_listTypes->Select(0, true);
		m_selectedItem = 0;
	}
	this->m_textTypeName->SetFocus();
}

void OpenTypeDlg::PopulateList()
{
	long item = m_listTypes->FindMatch(m_textTypeName->GetValue());
	if (item != wxNOT_FOUND) {
		// first unselect the current item
		if (m_selectedItem != wxNOT_FOUND && m_selectedItem != item) {
			m_listTypes->Select(m_selectedItem, false);
		}
		m_selectedItem = item;
		m_listTypes->Select(m_selectedItem);
		m_listTypes->EnsureVisible(m_selectedItem);
		m_listTypes->Focus(m_selectedItem);
	}
}

void OpenTypeDlg::OnCharHook(wxKeyEvent &event)
{
	if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
		
		m_selectedItem = m_listTypes->GetFirstSelected();
		if(m_selectedItem != wxNOT_FOUND)
			TryOpenAndEndModal();
		return;
		
	} else if (event.GetKeyCode() == WXK_DOWN) {
		long selectedItem = m_listTypes->GetFirstSelected();
		if (selectedItem == wxNOT_FOUND && m_listTypes->GetItemCount() > 0) {
			selectedItem = 0;
		}
		if (selectedItem == wxNOT_FOUND)
			return;

		//select the next one if we can
		if (m_listTypes->GetItemCount() > (selectedItem + 1)) {
			
#ifdef __WXMAC__
			// unselect current item
			m_listTypes->Select(m_selectedItem, false);
#endif
			
			//we can select the next one
			m_listTypes->Select(selectedItem + 1);
			
#ifdef __WXMAC__			
			m_listTypes->EnsureVisible(selectedItem+1);
#endif		
			m_listTypes->Focus(selectedItem+1);
			m_selectedItem = selectedItem+1;
			
		}
		return;
	} else if (event.GetKeyCode() == WXK_UP) {
		long selectedItem = m_listTypes->GetFirstSelected();
		if (selectedItem == wxNOT_FOUND && m_listTypes->GetItemCount() > 0) {
			selectedItem = 0;
		}
		if (selectedItem == wxNOT_FOUND)
			return;

		//select the previous one if we can
		if ((selectedItem - 1) >= 0) {
#ifdef __WXMAC__
			// unselect current item
			m_listTypes->Select(m_selectedItem, false);
#endif
			//we can select the next one
			m_listTypes->Select(selectedItem - 1);
			
#ifdef __WXMAC__			
			m_listTypes->EnsureVisible(selectedItem-1);
#endif		
			m_listTypes->Focus(selectedItem-1);
			m_selectedItem = selectedItem-1;
		}
		return;
	}
	event.Skip();
}

void OpenTypeDlg::TryOpenAndEndModal()
{
	TagEntryPtr t = m_listTypes->GetTagAt(m_selectedItem);
	if(t) {
		m_tag = t;
		// save the checkbox value into the settings
		EndModal(wxID_OK);
	} else {
		EndModal(wxID_CANCEL);
	}
}

void OpenTypeDlg::OnOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	m_selectedItem = m_listTypes->GetFirstSelected();
	if(m_selectedItem != wxNOT_FOUND)
		TryOpenAndEndModal();
}

void OpenTypeDlg::OnItemActivated(wxListEvent &event)
{
	wxUnusedVar(event);
	m_selectedItem = m_listTypes->GetFirstSelected();
	if(m_selectedItem != wxNOT_FOUND)
		TryOpenAndEndModal();
}

void OpenTypeDlg::OnAllowPartialMatch(wxCommandEvent& e)
{
	m_filter.empty();
	PopulateList();
	e.Skip();
}

void OpenTypeDlg::OnText(wxCommandEvent& e)
{
	PopulateList();
	e.Skip();
}
