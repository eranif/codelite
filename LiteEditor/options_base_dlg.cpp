//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : options_base_dlg.cpp
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
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingsgeneralpage.h"
#include "windowattrmanager.h"
#include "wx/wxprec.h"
#include <wx/fontmap.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "options_base_dlg.h"
#include "dialogspage.h"
#include <wx/dir.h>
#include "commentpage.h"
#include "frame.h"
#include "generalinfo.h"
#include "wx/notebook.h"
#include "lexer_configuration.h"
#include "lexer_page.h"
#include "editor_config.h"
#include "manager.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( OptionsDlg, wxDialog )
	EVT_BUTTON( wxID_OK, OptionsDlg::OnButtonOK )
	EVT_BUTTON( wxID_CANCEL, OptionsDlg::OnButtonCancel )
	EVT_BUTTON( wxID_APPLY, OptionsDlg::OnButtonApply )
END_EVENT_TABLE()

OptionsDlg::OptionsDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style )
		: wxDialog( parent, id, title, pos, wxSize(800, 600), style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->Centre( wxBOTH );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	long nbStyle = wxNB_DEFAULT;
	m_book = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, nbStyle );
	m_book->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	//create the general page
	m_book->AddPage( CreateGeneralPage(), wxT("General"), true );

	//create the folding page
	m_book->AddPage( CreateFoldingPage(), wxT("Folding"), false);

	//Create the bookmark page
	m_book->AddPage( CreateBookmarksPage(), wxT("Bookmarks"), false);

	//add C++ comment page
	m_book->AddPage( CreateCxxCommentPage(), wxT("C++ Comments"), false);

	m_book->AddPage( CreateDialogsPage(), wxT("Dialogs"), false);
	m_book->AddPage( CreateMiscPage(), wxT("Misc"), false );
	mainSizer->Add( m_book, 1, wxEXPAND | wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_okButton = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_okButton, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_cancelButton = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_cancelButton, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_applyButton = new wxButton( this, wxID_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_applyButton, 0, wxALIGN_RIGHT|wxALL, 5 );

	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );

	m_book->SetFocus();
	this->SetSizer( mainSizer );
	mainSizer->Fit(this);
	this->Layout();
	
	WindowAttrManager::Load(this, wxT("OptionsDlgAttr"), NULL);
}

OptionsDlg::~OptionsDlg()
{
	WindowAttrManager::Save(this, wxT("OptionsDlgAttr"), NULL);
}

wxPanel *OptionsDlg::CreateGeneralPage()
{
	m_genPage = new EditorSettingsGeneralPage( m_book );
	return m_genPage;
}

void OptionsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveChanges();
	ManagerST::Get()->ApplySettingsChanges();
	// and close the dialog
	EndModal(wxID_OK);
}

void OptionsDlg::OnButtonApply(wxCommandEvent &event)
{
	SaveChanges();
	ManagerST::Get()->ApplySettingsChanges();
	wxUnusedVar(event);
}

void OptionsDlg::OnButtonCancel(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void OptionsDlg::SaveChanges()
{
	if (m_checkBoxShowSplash->IsChecked()) {
		Frame::Get()->SetFrameFlag(true, CL_SHOW_SPLASH);
	} else {
		Frame::Get()->SetFrameFlag(false, CL_SHOW_SPLASH);
	}

	// construct an OptionsConfig object and update the configuration
	OptionsConfigPtr options(new OptionsConfig(NULL));
	
	// for performance reasons, we start a transaction for the configuration
	// file
	EditorConfigST::Get()->Begin();
	
	options->SetDisplayFoldMargin( m_checkBoxDisplayFoldMargin->IsChecked() );
	options->SetUnderlineFoldLine( m_checkBoxMarkFoldedLine->IsChecked() );
	options->SetFoldStyle(m_foldStyleChoice->GetStringSelection());
	options->SetDisplayBookmarkMargin(m_displayBookmarkMargin->IsChecked());
	options->SetBookmarkShape( m_bookmarkShape->GetStringSelection());
	options->SetBookmarkBgColour( m_bgColourPicker->GetColour() );
	options->SetBookmarkFgColour( m_fgColourPicker->GetColour() );
	options->SetFoldAtElse(m_foldAtElse->IsChecked());
	options->SetFoldCompact(m_foldCompact->IsChecked());
	options->SetFoldPreprocessor(m_foldPreprocessor->IsChecked());
	
	int iconSize(24);
	if (m_iconSize->GetStringSelection() == wxT("Toolbar uses small icons (16x16)")) {
		iconSize = 16;
	}
	
	options->SetIconsSize(iconSize);
	// save file font encoding
	options->SetFileFontEncoding(m_fileFontEncoding->GetStringSelection());
	
	
	int value = m_findReplaceHistory->GetValue();
	if(value < 1 || value > 50) {
		value = 10;
	}
	EditorConfigST::Get()->SaveLongValue(wxT("MaxItemsInFindReplaceDialog"), value);
	
	// save the WordHighlightColour value
	EditorConfigST::Get()->SaveStringValue(wxT("WordHighlightColour"), m_wordHighlightColour->GetColour().GetAsString());
	EditorConfigST::Get()->SaveLongValue(wxT("SingleInstance"), m_singleInstance->IsChecked() ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("CheckNewVersion"), m_checkForNewVersion->IsChecked() ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("ShowFullPathInFrameTitle"), m_checkFullPathInTitle->IsChecked() ? 1 : 0);
	
	// apply the title format changes
	Frame::Get()->SetFrameTitle(ManagerST::Get()->GetActiveEditor());
	
	//check to see of the icon size was modified
	int oldIconSize(24);
	OptionsConfigPtr oldOptions = EditorConfigST::Get()->GetOptions();
	if (oldOptions) {
		oldIconSize = oldOptions->GetIconsSize();
	}
	if (oldIconSize != iconSize) {
		EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 0);
		//notify the user
		wxMessageBox(_("Toolbar icons size change, requires restart of CodeLite"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
	} else {
		EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 1);
	}

	m_commentPage->Save();
	m_dialogsPage->Save();
	m_genPage->Save(options); 
	
	EditorConfigST::Get()->SetOptions(options);
	
	// save the modifications to the disk
	EditorConfigST::Get()->Save();
	
	ManagerST::Get()->ApplySettingsChanges();
}

wxPanel* OptionsDlg::CreateBookmarksPage()
{
	//get the editor's options from the disk
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();

	wxPanel *page = new wxPanel(m_book, wxID_ANY);
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	page->SetSizer(sz);

	m_displayBookmarkMargin = new wxCheckBox(page, wxID_ANY, wxT("Display Selection / Bookmark margin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_displayBookmarkMargin->SetValue(options->GetDisplayBookmarkMargin());

	sz->Add( m_displayBookmarkMargin, 0, wxALL, 5 );

	m_staticText6 = new wxStaticText( page, wxID_ANY, wxT("Bookmark Shape:"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add( m_staticText6, 0, wxALL, 5 );

	wxString m_bookmarkShapeChoices[] = { wxT("Small Rectangle"), wxT("Rounded Rectangle"), wxT("Circle"), wxT("Small Arrow") };
	int m_bookmarkShapeNChoices = sizeof( m_bookmarkShapeChoices ) / sizeof( wxString );
	m_bookmarkShape = new wxChoice( page, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_bookmarkShapeNChoices, m_bookmarkShapeChoices, 0 );
	sz->Add( m_bookmarkShape, 0, wxALL|wxEXPAND, 5 );
	m_bookmarkShape->SetStringSelection(options->GetBookmarkShape());

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 3, 2, 0, 0 );

	m_staticText4 = new wxStaticText( page, wxID_ANY, wxT("Select the bookmark background colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_bgColourPicker = new wxColourPickerCtrl(page, wxID_ANY, options->GetBookmarkBgColour(), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gSizer1->Add( m_bgColourPicker, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_staticText5 = new wxStaticText( page, wxID_ANY, wxT("Select the bookmark forground colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_fgColourPicker = new wxColourPickerCtrl( page, wxID_ANY, options->GetBookmarkFgColour(), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gSizer1->Add( m_fgColourPicker, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxStaticText *t1 = new wxStaticText( page, wxID_ANY, wxT("Select word highlight colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( t1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxColour col1(wxT("LIGHT BLUE"));
	wxString val1 = EditorConfigST::Get()->GetStringValue(wxT("WordHighlightColour"));
	if (val1.IsEmpty() == false) {
		col1 = wxColour(val1);
	}
	m_wordHighlightColour = new wxColourPickerCtrl(page, wxID_ANY, col1, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gSizer1->Add( m_wordHighlightColour, 0, wxALIGN_RIGHT|wxALL, 5 );

	sz->Add( gSizer1, 0, wxEXPAND|wxALL, 5 );

	return page;

}

wxPanel* OptionsDlg::CreateFoldingPage()
{
	//get the editor's options from the disk
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();

	wxPanel *page = new wxPanel(m_book, wxID_ANY);
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	page->SetSizer(sz);

	m_checkBoxDisplayFoldMargin = new wxCheckBox( page, wxID_ANY, wxT("Display Folding Margin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxDisplayFoldMargin->SetValue(options->GetDisplayFoldMargin());

	sz->Add( m_checkBoxDisplayFoldMargin, 0, wxALL, 5 );

	m_checkBoxMarkFoldedLine = new wxCheckBox( page, wxID_ANY, wxT("Underline Folded Line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxMarkFoldedLine->SetValue(options->GetUnderlineFoldLine());
	sz->Add( m_checkBoxMarkFoldedLine, 0, wxALL, 5 );
	
	m_foldPreprocessor = new wxCheckBox( page, wxID_ANY, wxT("Fold Preprocessors"), wxDefaultPosition, wxDefaultSize, 0 );
	m_foldPreprocessor->SetValue(options->GetFoldPreprocessor());
	sz->Add( m_foldPreprocessor, 0, wxALL, 5 );
	
	m_foldCompact = new wxCheckBox( page, wxID_ANY, wxT("Fold Compact"), wxDefaultPosition, wxDefaultSize, 0 );
	m_foldCompact->SetValue(options->GetFoldCompact());
	sz->Add( m_foldCompact, 0, wxALL, 5 );
	
	m_foldAtElse = new wxCheckBox( page, wxID_ANY, wxT("Fold At Else"), wxDefaultPosition, wxDefaultSize, 0 );
	m_foldAtElse->SetValue(options->GetFoldAtElse());
	sz->Add( m_foldAtElse, 0, wxALL, 5 );
	
	m_staticText1 = new wxStaticText( page, wxID_ANY, wxT("Fold Style:"), wxDefaultPosition, wxDefaultSize, 0 );
	sz->Add( m_staticText1, 0, wxALL, 5 );

	wxString m_foldStyleChoiceChoices[] = { wxT("Simple"), wxT("Arrows"), wxT("Flatten Tree Square Headers"), wxT("Flatten Tree Circular Headers") };
	int m_foldStyleChoiceNChoices = sizeof( m_foldStyleChoiceChoices ) / sizeof( wxString );
	m_foldStyleChoice = new wxChoice( page, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_foldStyleChoiceNChoices, m_foldStyleChoiceChoices, 0 );
	sz->Add( m_foldStyleChoice, 0, wxALL|wxEXPAND, 5 );
	m_foldStyleChoice->SetStringSelection( options->GetFoldStyle() );

	return page;
}

wxPanel* OptionsDlg::CreateCxxCommentPage()
{
	m_commentPage = new CommentPage(m_book);
	return m_commentPage;
}

wxPanel* OptionsDlg::CreateMiscPage()
{
	GeneralInfo info = Frame::Get()->GetFrameGeneralInfo();

	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_miscPage = new wxPanel( m_book, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* vSz1;
	vSz1 = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *bszier = new wxBoxSizer(wxVERTICAL);

	wxString iconSize[] = { wxT("Toolbar uses small icons (16x16)"), wxT("Toolbar uses large icons (24x24)") };
	m_iconSize = new wxChoice( m_miscPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, iconSize, 0 );
	bszier->Add( m_iconSize, 0, wxALL|wxEXPAND, 5 );
	
	if (options->GetIconsSize() == 16) {
		m_iconSize->SetSelection(0);
	} else {
		m_iconSize->SetSelection(1);
	}

	// file font encoding
	wxBoxSizer *hsEnc = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *txtEnc = new wxStaticText( m_miscPage, wxID_ANY, wxT("File font encoding"), wxDefaultPosition, wxDefaultSize, 0 );
	hsEnc->Add(txtEnc, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxArrayString astrEncodings;
	wxFontEncoding fontEnc;
	int iCurrSelId = 0;
	size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
	for (size_t i = 0; i < iEncCnt; i++) {
		fontEnc = wxFontMapper::GetEncoding(i);
		if (wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
			continue;
		}
		astrEncodings.Add(wxFontMapper::GetEncodingName(fontEnc));
		if (fontEnc == options->GetFileFontEncoding()) {
			iCurrSelId = i;
		}
	}
	m_fileFontEncoding = new wxChoice( m_miscPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, astrEncodings, 0 );
	m_fileFontEncoding->SetSelection(iCurrSelId);
	hsEnc->Add( m_fileFontEncoding, 1, wxALL|wxEXPAND, 5 );

	bszier->Add( hsEnc, 0, wxEXPAND);

	m_checkBoxShowSplash = new wxCheckBox( m_miscPage, wxID_ANY, wxT("Show splashscreen on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	bszier->Add(m_checkBoxShowSplash, 0, wxEXPAND | wxALL, 5);

	m_singleInstance = new wxCheckBox(m_miscPage, wxID_ANY, wxT("Allow only single instance running"), wxDefaultPosition, wxDefaultSize, 0);
	bszier->Add(m_singleInstance, 0, wxEXPAND | wxALL, 5);
	
	m_checkForNewVersion = new wxCheckBox(m_miscPage, wxID_ANY, wxT("Check for new version on startup"), wxDefaultPosition, wxDefaultSize, 0);
	bszier->Add(m_checkForNewVersion, 0, wxEXPAND | wxALL, 5);
	
	m_checkFullPathInTitle = new wxCheckBox(m_miscPage, wxID_ANY, wxT("Show file's full path in frame title"), wxDefaultPosition, wxDefaultSize, 0);
	bszier->Add(m_checkFullPathInTitle, 0, wxEXPAND | wxALL, 5);
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText *txthistory = new wxStaticText( m_miscPage, wxID_ANY, wxT("Clear Recent workspace / files history"), wxDefaultPosition, wxDefaultSize, 0 );
	txthistory->Wrap( -1 );
	bSizer2->Add( txthistory, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonClearHistory = new wxButton( m_miscPage, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonClearHistory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bszier->Add(bSizer2, 0, wxEXPAND | wxALL, 5);
	
	wxBoxSizer* hs1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText *txtHistory = new wxStaticText( m_miscPage, wxID_ANY, wxT("Max items kept in find / replace dialog:"), wxDefaultPosition, wxDefaultSize, 0 );
	txtHistory->Wrap( -1 );
	hs1->Add( txtHistory, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_findReplaceHistory = new wxSpinCtrl( m_miscPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 50, 10 );
	hs1->Add( m_findReplaceHistory, 0, wxALL|wxEXPAND, 5 );
	bszier->Add(hs1, 0, wxEXPAND | wxALL, 5);
	
	ConnectButton(m_buttonClearHistory, OptionsDlg::OnClearHistory);
	// Connect Events
	m_buttonClearHistory->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( OptionsDlg::OnClearHistoryUI ), NULL, this );
	
	long single_instance(1);
	EditorConfigST::Get()->GetLongValue(wxT("SingleInstance"), single_instance);
	m_singleInstance->SetValue(single_instance ? true : false);
	
	long check(1);
	EditorConfigST::Get()->GetLongValue(wxT("CheckNewVersion"), check);
	m_checkForNewVersion->SetValue(check ? true : false);

	check = 1;
	EditorConfigST::Get()->GetLongValue(wxT("ShowFullPathInFrameTitle"), check);
	m_checkFullPathInTitle->SetValue(check ? true : false);

	bool showSplash = info.GetFlags() & CL_SHOW_SPLASH ? true : false;
	m_checkBoxShowSplash->SetValue(showSplash);

	long max_items(10);
	EditorConfigST::Get()->GetLongValue(wxT("MaxItemsInFindReplaceDialog"), max_items);
	m_findReplaceHistory->SetValue(max_items);
	
	vSz1->Add( bszier, 0, wxEXPAND, 5 );
	
	m_miscPage->SetSizer( vSz1 );
	m_miscPage->Layout();
	vSz1->Fit( m_miscPage );
	return m_miscPage;
}

void OptionsDlg::OnClearHistory(wxCommandEvent& event)
{
	wxUnusedVar(event);
	ManagerST::Get()->ClearFileHistory();
}

void OptionsDlg::OnClearHistoryUI(wxUpdateUIEvent& e)
{
	e.Enable(ManagerST::Get()->HasHistory());
}

wxPanel* OptionsDlg::CreateDialogsPage()
{
	m_dialogsPage = new DialogsPage(m_book);
	return m_dialogsPage;
}
