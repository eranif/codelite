///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "options_base_dlg.h"
#include "frame.h"
#include "generalinfo.h"
#include "wx/notebook.h"
#include "lexer_configuration.h"
#include "lexer_page.h"
#include "editor_config.h"
#include "manager.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( OptionsDlg, wxDialog )
	EVT_BUTTON( wxID_OK, OptionsDlg::OnButtonOK )
	EVT_BUTTON( wxID_CANCEL, OptionsDlg::OnButtonCancel )
	EVT_BUTTON( wxID_APPLY, OptionsDlg::OnButtonApply )
END_EVENT_TABLE()

OptionsDlg::OptionsDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, wxSize(700, 600), style )
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

	m_book->AddPage( CreateSyntaxHighlightPage(), wxT("Syntax Highlight"), false );
	
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
	this->Layout();
}

wxPanel *OptionsDlg::CreateSyntaxHighlightPage()
{
	wxPanel *page = new wxPanel( m_book, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	page->SetSizer(sz);

	long style = wxNB_DEFAULT;
	m_lexersBook = new wxNotebook(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	sz->Add(m_lexersBook, 1, wxEXPAND | wxALL, 5);
	m_lexersBook->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	bool selected = true;
	EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
	for(; iter != EditorConfigST::Get()->LexerEnd(); iter++){
		LexerConfPtr lexer = iter->second;
		m_lexersBook->AddPage(CreateLexerPage(m_lexersBook, lexer), lexer->GetName(), selected);
		selected = false;
	}

	return page;
}

wxPanel *OptionsDlg::CreateGeneralPage()
{
	GeneralInfo info = Frame::Get()->GetFrameGeneralInfo();;
		
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
	
	wxString iconSize[] = { wxT("Toolbar uses small icons (16x16)"), wxT("Toolbar uses large icons (24x24)") };
	m_iconSize = new wxChoice( m_general, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, iconSize, 0 );
	bszier->Add( m_iconSize, 0, wxALL|wxEXPAND, 5 );
	
	if(options->GetIconsSize() == 16) {
		m_iconSize->SetSelection(0);
	}else{
		m_iconSize->SetSelection(1);
	}
	
	//set some colour pickers
	wxStaticLine *line = new wxStaticLine( m_general, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bszier->Add(line, 0, wxEXPAND | wxALL, 5);
	
	m_checkBoxShowSplash = new wxCheckBox( m_general, wxID_ANY, wxT("Show splashscreen on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	bszier->Add(m_checkBoxShowSplash, 0, wxEXPAND | wxALL, 5);
	bool showSplash = info.GetFlags() & CL_SHOW_SPLASH ? true : false;
	m_checkBoxShowSplash->SetValue(showSplash);
	
	line = new wxStaticLine( m_general, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bszier->Add(line, 0, wxEXPAND | wxALL, 5);
	
	wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *txt = new wxStaticText(m_general, wxID_ANY, wxT("Select the caret line background colour:"));
	hsizer->Add(txt, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5);

	m_caretLineColourPicker = new wxColourPickerCtrl(m_general, wxID_ANY, options->GetCaretLineColour(), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	hsizer->Add(m_caretLineColourPicker, 0, wxALL|wxEXPAND, 5);
	bszier->Add( hsizer, 0, wxEXPAND);
	
	hsizer = new wxBoxSizer(wxHORIZONTAL);
	txt = new wxStaticText(m_general, wxID_ANY, wxT("Select the caret forground colour:"));
	hsizer->Add(txt, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5);

	m_caretColourPicker = new wxColourPickerCtrl(m_general, wxID_ANY, options->GetCaretColour(), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	hsizer->Add(m_caretColourPicker, 0, wxALL|wxEXPAND, 5);
	bszier->Add( hsizer, 0, wxEXPAND);
	
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
	wxUnusedVar(event);
}

void OptionsDlg::OnButtonCancel(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void OptionsDlg::SaveChanges()
{
	int max = m_lexersBook->GetPageCount();
	for(int i=0; i<max; i++){
		wxWindow *win = m_lexersBook->GetPage((size_t)i);
		LexerPage *page = dynamic_cast<LexerPage*>( win );
		if( page ){
			page->SaveSettings();
		}
	}
	
	if(m_checkBoxShowSplash->IsChecked()){
		Frame::Get()->SetFrameFlag(true, CL_SHOW_SPLASH);
	}else{
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
	options->SetCaretLineColour(m_caretLineColourPicker->GetColour());
	options->SetCaretColour(m_caretColourPicker->GetColour());
	options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
	
	int iconSize(24);
	if(m_iconSize->GetStringSelection() == wxT("Toolbar uses small icons (16x16)")) {
		iconSize = 16;
	}
	options->SetIconsSize(iconSize);
	
	//check to see of the icon size was modified
	int oldIconSize(24);
	OptionsConfigPtr oldOptions = EditorConfigST::Get()->GetOptions();
	if(oldOptions) {
		oldIconSize = oldOptions->GetIconsSize();
	}
	if(oldIconSize != iconSize) {
		EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 0);
		//notify the user
		wxMessageBox(wxT("Toolbar icons size change, requires restart of CodeLite"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
	}else{
		EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 1);
	}
	
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
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticText4 = new wxStaticText( page, wxID_ANY, wxT("Select the bookmark background colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_bgColourPicker = new wxColourPickerCtrl(page, wxID_ANY, options->GetBookmarkBgColour(), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gSizer1->Add( m_bgColourPicker, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticText5 = new wxStaticText( page, wxID_ANY, wxT("Select the bookmark forground colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_fgColourPicker = new wxColourPickerCtrl( page, wxID_ANY, options->GetBookmarkFgColour(), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gSizer1->Add( m_fgColourPicker, 0, wxALIGN_RIGHT|wxALL, 5 );
	
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
