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

#include "wx/wxprec.h"
#include <wx/fontmap.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "options_base_dlg.h"
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

	m_book->AddPage( CreateSyntaxHighlightPage(), wxT("Syntax Highlight"), false );
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
}

wxPanel *OptionsDlg::CreateSyntaxHighlightPage()
{
	wxPanel *page = new wxPanel( m_book, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	page->SetSizer(sz);

	wxArrayString themesArr;

	wxString path = ManagerST::Get()->GetStarupDirectory();
	path << wxT("/lexers/");

	wxArrayString files;
	wxArrayString dirs;
	wxDir::GetAllFiles(path, &files, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	//filter out all non-directories
	wxFileName base_path( path );
	for (size_t i=0; i<files.GetCount(); i++) {
		wxFileName fn( files.Item(i) );
		wxString new_path( fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) );
		if (new_path != base_path.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR)) {
			fn.MakeRelativeTo(base_path.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			new_path = fn.GetPath();
			if (dirs.Index(new_path) == wxNOT_FOUND) {
				dirs.Add(new_path);
			}
		}
	}
	wxStaticText *txt = new wxStaticText(page, wxID_ANY, wxT("Colouring scheme:"), wxDefaultPosition, wxDefaultSize, 0);
	sz->Add(txt, 0, wxEXPAND|wxALL, 5);
	
	m_themes = new wxChoice(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, dirs, 0 );
	sz->Add(m_themes, 0, wxEXPAND|wxALL, 5);

	if (m_themes->IsEmpty() == false) {
		int where = m_themes->FindString(EditorConfigST::Get()->GetStringValue( wxT("LexerTheme") ));
		if ( where != wxNOT_FOUND) {
			m_themes->SetSelection( where );
		}
	}

	long style = wxNB_DEFAULT;
	m_lexersBook = new wxNotebook(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	sz->Add(m_lexersBook, 1, wxEXPAND | wxALL, 5);
	m_lexersBook->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	LoadLexers(m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection());

	m_startingTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
	ConnectChoice(m_themes, OptionsDlg::OnThemeChanged);
	return page;
}

wxPanel *OptionsDlg::CreateGeneralPage()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_general = new wxPanel( m_book, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* vSz1;
	vSz1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer *bszier = new wxBoxSizer(wxVERTICAL);

	m_displayLineNumbers = new wxCheckBox( m_general, wxID_ANY, wxT("Display Line Numbers"), wxDefaultPosition, wxDefaultSize, 0 );
	m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
	bszier->Add( m_displayLineNumbers, 0, wxALL, 5 );

	m_indentsUsesTabs = new wxCheckBox( m_general, wxID_ANY, wxT("Use Tabs For Indentation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
	bszier->Add( m_indentsUsesTabs, 0, wxALL, 5 );

	m_showIndentationGuideLines = new wxCheckBox( m_general, wxID_ANY, wxT("Show Indentation Guidelines"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());
	bszier->Add( m_showIndentationGuideLines, 0, wxALL, 5 );

	m_highlighyCaretLine = new wxCheckBox( m_general, wxID_ANY, wxT("Highlight Caret Line"), wxDefaultPosition, wxDefaultSize, 0);
	m_highlighyCaretLine->SetValue(options->GetHighlightCaretLine());
	bszier->Add( m_highlighyCaretLine, 0, wxALL, 5 );

	//set some colour pickers
	wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *txt = new wxStaticText(m_general, wxID_ANY, wxT("Select the caret line background colour:"));
	hsizer->Add(txt, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_caretLineColourPicker = new wxColourPickerCtrl(m_general, wxID_ANY, options->GetCaretLineColour(), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	hsizer->Add(m_caretLineColourPicker, 0, wxALL|wxEXPAND, 5);
	bszier->Add( hsizer, 1, wxEXPAND);

	wxStaticLine *line = new wxStaticLine( m_general, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bszier->Add(line, 0, wxEXPAND);

	wxBoxSizer *hs1 = new wxBoxSizer(wxHORIZONTAL);
	txt = new wxStaticText( m_general, wxID_ANY, wxT("Editor Tab Width:"), wxDefaultPosition, wxDefaultSize, 0 );
	hs1->Add(txt, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_spinCtrlTabWidth = new wxSpinCtrl( m_general, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 8, 4 );
	hs1->Add( m_spinCtrlTabWidth, 1, wxALL, 5 );
	
	wxBoxSizer *hs2 = new wxBoxSizer(wxHORIZONTAL);
	txt = new wxStaticText( m_general, wxID_ANY, wxT("Whitespaces Visibility:"), wxDefaultPosition, wxDefaultSize, 0 );
	hs2->Add(txt, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	wxArrayString styles;
	styles.Add(wxT("Invisible"));
	styles.Add(wxT("Visible always"));
	styles.Add(wxT("Visible after indent"));
	
	m_whitespaceStyle = new wxChoice(m_general, wxID_ANY, wxDefaultPosition, wxDefaultSize, styles, 0);
	hs2->Add(m_whitespaceStyle, 1, wxEXPAND|wxALL, 5);
	switch(options->GetShowWhitspaces()) {
		case 0:
			m_whitespaceStyle->SetStringSelection(wxT("Invisible"));
			break;
		case 1:
			m_whitespaceStyle->SetStringSelection(wxT("Visible always"));
			break;
		case 2:
			m_whitespaceStyle->SetStringSelection(wxT("Visible after indent"));
			break;
		default:
			m_whitespaceStyle->SetStringSelection(wxT("Invisible"));
			break;
	}
	
	long value(4);
	EditorConfigST::Get()->GetLongValue(wxT("EditorTabWidth"), value);
	m_spinCtrlTabWidth->SetValue(value);

	bszier->Add(hs1, 0, wxEXPAND);
	bszier->Add(hs2, 0, wxEXPAND);
	vSz1->Add( bszier, 0, wxEXPAND, 5 );

	m_general->SetSizer( vSz1 );
	m_general->Layout();
	vSz1->Fit( m_general );
	return m_general;
}

wxPanel *OptionsDlg::CreateLexerPage(wxWindow *parent, LexerConfPtr lexer)
{
	return new LexerPage(parent, lexer);
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

	m_startingTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
	wxUnusedVar(event);
}

void OptionsDlg::OnButtonCancel(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString curSelTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
	if (curSelTheme != m_startingTheme) {
		//restore the starting theme
		EditorConfigST::Get()->SaveStringValue(wxT("LexerTheme"), m_startingTheme);
		EditorConfigST::Get()->LoadLexers();
	}

	EndModal(wxID_CANCEL);
}

void OptionsDlg::SaveChanges()
{

	int max = m_lexersBook->GetPageCount();
	for (int i=0; i<max; i++) {
		wxWindow *win = m_lexersBook->GetPage((size_t)i);
		LexerPage *page = dynamic_cast<LexerPage*>( win );
		if ( page ) {
			page->SaveSettings();
		}
	}

	if (m_checkBoxShowSplash->IsChecked()) {
		Frame::Get()->SetFrameFlag(true, CL_SHOW_SPLASH);
	} else {
		Frame::Get()->SetFrameFlag(false, CL_SHOW_SPLASH);
	}

	// construct an OptionsConfig object and update the configuration
	OptionsConfigPtr options(new OptionsConfig(NULL));
	options->SetDisplayFoldMargin( m_checkBoxDisplayFoldMargin->IsChecked() );
	options->SetUnderlineFoldLine( m_checkBoxMarkFoldedLine->IsChecked() );
	options->SetFoldStyle(m_foldStyleChoice->GetStringSelection());
	options->SetDisplayBookmarkMargin(m_displayBookmarkMargin->IsChecked());
	options->SetBookmarkShape( m_bookmarkShape->GetStringSelection());
	options->SetBookmarkBgColour( m_bgColourPicker->GetColour() );
	options->SetBookmarkFgColour( m_fgColourPicker->GetColour() );
	options->SetHighlightCaretLine( m_highlighyCaretLine->IsChecked() );
	options->SetDisplayLineNumbers( m_displayLineNumbers->IsChecked() );
	options->SetShowIndentationGuidelines( m_showIndentationGuideLines->IsChecked() );
	options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
	options->SetCaretLineColour(m_caretLineColourPicker->GetColour());

	int iconSize(24);
	if (m_iconSize->GetStringSelection() == wxT("Toolbar uses small icons (16x16)")) {
		iconSize = 16;
	}
	
	options->SetIconsSize(iconSize);
	// save file font encoding
	options->SetFileFontEncoding(m_fileFontEncoding->GetStringSelection());
	
	// save the tab width
	int value = m_spinCtrlTabWidth->GetValue();

	// make sure we are saving correct values
	if (value < 1 || value > 8) {
		value = 4;
	}
	// save it to configuration file
	EditorConfigST::Get()->SaveLongValue(wxT("EditorTabWidth"), value);
	
	value = m_findReplaceHistory->GetValue();
	if(value < 1 || value > 50) {
		value = 10;
	}
	EditorConfigST::Get()->SaveLongValue(wxT("MaxItemsInFindReplaceDialog"), value);
	
	// save the whitespace visibility
	int style(0); // inivisble
	if(m_whitespaceStyle->GetStringSelection() == wxT("Visible always")) {
		style = 1;
	}else if(m_whitespaceStyle->GetStringSelection() == wxT("Visible after indent")) {
		style = 2;
	}
	
	options->SetShowWhitspaces(style);
	
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
		wxMessageBox(wxT("Toolbar icons size change, requires restart of CodeLite"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
	} else {
		EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 1);
	}

	m_commentPage->Save();

	EditorConfigST::Get()->SetOptions(options);
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

void OptionsDlg::LoadLexers(const wxString& theme)
{
	Freeze();
	bool selected = true;

	//remove old lexers
	if (m_lexersBook->GetPageCount() > 0) {
		m_lexersBook->DeleteAllPages();
	}

	//update the theme name
	EditorConfigST::Get()->SaveStringValue(wxT("LexerTheme"), theme);

	//load all lexers
	EditorConfigST::Get()->LoadLexers();

	EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
	for (; iter != EditorConfigST::Get()->LexerEnd(); iter++) {
		LexerConfPtr lexer = iter->second;
		m_lexersBook->AddPage(CreateLexerPage(m_lexersBook, lexer), lexer->GetName(), selected);
		selected = false;
	}
	Thaw();
}

void OptionsDlg::OnThemeChanged(wxCommandEvent& event)
{
	int sel = event.GetSelection();
	wxString themeName = m_themes->GetString((unsigned int)sel);

	//update the configuration with the new lexer's theme
	EditorConfigST::Get()->SaveStringValue(wxT("LexerTheme"), themeName);

	LoadLexers( themeName );
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
