#include "envvar_table.h"
#include <wx/wupdlock.h>
#include "evnvarlist.h"
#include "environmentconfig.h"
#include "windowattrmanager.h"

EnvVarsTableDlg::EnvVarsTableDlg( wxWindow* parent )
		: EnvVarsTableDlgBase( parent )
{
	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	std::map<wxString, wxString> envSets = vars.GetEnvVarSets();
	wxString activePage = vars.GetActiveSet();
	
	wxScintilla *sci = m_textCtrlDefault;
	sci->StyleClearAll();
	sci->SetLexer(wxSCI_LEX_NULL);

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	for (int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		sci->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		sci->StyleSetForeground(i, *wxBLACK);
		sci->StyleSetFont(i, font);
	}
	
	WindowAttrManager::Load(this, wxT("EnvVarsTableDlg"), NULL);
	std::map<wxString, wxString>::iterator iter = envSets.begin();
	for (; iter != envSets.end(); iter++) {
		wxString name    = iter->first;
		wxString content = iter->second;

		if (name == wxT("Default")) {
			m_textCtrlDefault->SetText(content);
		} else {
			EnvVarSetPage *page = new EnvVarSetPage(m_notebook1);
			page->m_textCtrl->SetText(content);
			m_notebook1->AddPage(page, name);
		}
	}

	m_notebook1->SetSelection(0);
	for (size_t i=0; i<m_notebook1->GetPageCount(); i++) {
		if (m_notebook1->GetPageText(i) == activePage) {
			m_notebook1->GetPage(i)->SetFocus();
			m_notebook1->SetSelection(i);
			break;
		}
	}
}

EnvVarsTableDlg::~EnvVarsTableDlg()
{
	WindowAttrManager::Save(this, wxT("EnvVarsTableDlg"), NULL);
}

void EnvVarsTableDlg::OnLeftUp( wxMouseEvent& event )
{
}

void EnvVarsTableDlg::OnButtonOk( wxCommandEvent& event )
{
	EvnVarList vars;

	std::map<wxString, wxString> envSets;

	wxString content = m_textCtrlDefault->GetText();
	wxString name    = wxT("Default");
	content.Trim().Trim(false);
	envSets[name] = content;

	for (size_t i=1; i<m_notebook1->GetPageCount(); i++) {
		if (i == (size_t)m_notebook1->GetSelection()) {
			vars.SetActiveSet(m_notebook1->GetPageText(i));
		}

		EnvVarSetPage *page = (EnvVarSetPage*) m_notebook1->GetPage(i);
		wxString content = page->m_textCtrl->GetText();
		wxString name    = m_notebook1->GetPageText(i);
		content.Trim().Trim(false);
		envSets[name] = content;
	}
	vars.SetEnvVarSets(envSets);
	EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
	event.Skip();
}

void EnvVarsTableDlg::OnNewSet( wxCommandEvent& event )
{
	wxString name = wxGetTextFromUser(wxT("Enter Name:"));
	if(name.IsEmpty())
		return;

	DoAddPage(name, false);
}

void EnvVarsTableDlg::OnDeleteSet(wxCommandEvent& event)
{
	wxUnusedVar(event);

	int selection = m_notebook1->GetSelection();
	if(selection == wxNOT_FOUND)
		return;

	wxString name = m_notebook1->GetPageText((size_t)selection);
	if(wxMessageBox(wxString::Format(wxT("Are you sure you want to delete '%s' environment variables set?"), name.c_str()), wxT("Confirm"), wxYES_NO|wxICON_QUESTION) != wxYES)
		return;
	m_notebook1->DeletePage((size_t)selection);
}

void EnvVarsTableDlg::OnDeleteSetUI(wxUpdateUIEvent& event)
{
	int i = m_notebook1->GetSelection();
	event.Enable(i != wxNOT_FOUND && m_notebook1->GetPageText(i) != wxT("Default"));
}

void EnvVarsTableDlg::DoAddPage(const wxString& name, bool select)
{
	wxWindowUpdateLocker locker(this);
	EnvVarSetPage *page = new EnvVarSetPage(m_notebook1, wxID_ANY, wxDefaultPosition, wxSize(0,0));
	wxScintilla *sci = page->m_textCtrl;
	sci->StyleClearAll();
	sci->SetLexer(wxSCI_LEX_NULL);

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	for (int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		sci->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		sci->StyleSetForeground(i, *wxBLACK);
		sci->StyleSetFont(i, font);
	}
	m_notebook1->AddPage(page, name, select);
}
