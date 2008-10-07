#include "lexer_page.h"
#include "windowattrmanager.h"
#include <wx/notebook.h>
#include "macros.h"
#include "editor_config.h"
#include <wx/dir.h>
#include "syntaxhighlightdlg.h"
#include "manager.h"

SyntaxHighlightDlg::SyntaxHighlightDlg( wxWindow* parent )
:
SyntaxHighlightBaseDlg( parent )
{
	GetSizer()->Insert(0, CreateSyntaxHighlightPage(), 1, wxALL|wxEXPAND, 5);
	GetSizer()->Layout();
	GetSizer()->Fit(this);
	m_buttonOk->SetFocus();
	WindowAttrManager::Load(this, wxT("SyntaxHighlightDlgAttr"), NULL);
}

void SyntaxHighlightDlg::OnButtonOK( wxCommandEvent& event )
{
	wxUnusedVar(event);
	SaveChanges();
	ManagerST::Get()->ApplySettingsChanges();
	// and close the dialog
	EndModal(wxID_OK);
}

void SyntaxHighlightDlg::OnButtonCancel( wxCommandEvent& event )
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

void SyntaxHighlightDlg::OnButtonApply( wxCommandEvent& event )
{
	SaveChanges();
	ManagerST::Get()->ApplySettingsChanges();
	
	m_startingTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
	wxUnusedVar(event);
}

wxPanel *SyntaxHighlightDlg::CreateSyntaxHighlightPage()
{
	wxPanel *page = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
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
	ConnectChoice(m_themes, SyntaxHighlightDlg::OnThemeChanged);
	return page;
}

void SyntaxHighlightDlg::LoadLexers(const wxString& theme)
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

void SyntaxHighlightDlg::OnThemeChanged(wxCommandEvent& event)
{
	int sel = event.GetSelection();
	wxString themeName = m_themes->GetString((unsigned int)sel);

	//update the configuration with the new lexer's theme
	EditorConfigST::Get()->SaveStringValue(wxT("LexerTheme"), themeName);

	LoadLexers( themeName );
}

wxPanel *SyntaxHighlightDlg::CreateLexerPage(wxWindow *parent, LexerConfPtr lexer)
{
	return new LexerPage(parent, lexer);
}

void SyntaxHighlightDlg::SaveChanges()
{
	int max = m_lexersBook->GetPageCount();
	for (int i=0; i<max; i++) {
		wxWindow *win = m_lexersBook->GetPage((size_t)i);
		LexerPage *page = dynamic_cast<LexerPage*>( win );
		if ( page ) {
			page->SaveSettings();
		}
	}
}

SyntaxHighlightDlg::~SyntaxHighlightDlg()
{
	WindowAttrManager::Save(this, wxT("SyntaxHighlightDlgAttr"), NULL);
}

