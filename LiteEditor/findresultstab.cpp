#include "findresultstab.h"
#include "wx/string.h"
#include "manager.h"

FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name)
: OutputTabWindow(parent, id, name)
{
	//SCLEX_FIF
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	m_sci->SetLexer(wxSCI_LEX_FIF);
	m_sci->StyleSetForeground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	
	m_sci->StyleSetForeground(wxSCI_LEX_FIF_FILE, wxT("BLUE"));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_FILE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	
	font.SetWeight(wxBOLD);
	m_sci->StyleSetFont(wxSCI_LEX_FIF_FILE, font);
	
	font.SetWeight(wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_FIF_DEFAULT, font);
	
	m_sci->StyleSetHotSpot(wxSCI_LEX_FIF_FILE, true);
}

FindResultsTab::~FindResultsTab()
{
}

void FindResultsTab::OnHotspotClicked(wxScintillaEvent &event)
{
	//same as double click
	OnMouseDClick(event);
}

void FindResultsTab::OnMouseDClick(wxScintillaEvent &event)
{
	long pos = event.GetPosition();
	int line = m_sci->LineFromPosition(pos);
	wxString lineText = m_sci->GetLine(line);

	//remove selection
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);
	
	// each line has the format of 
	// file(line, col): text
	wxString fileName = lineText.BeforeFirst(wxT('('));
	wxString strLineNumber = lineText.AfterFirst(wxT('('));
	strLineNumber = strLineNumber.BeforeFirst(wxT(','));
	strLineNumber = strLineNumber.Trim();
	long lineNumber = -1;
	strLineNumber.ToLong(&lineNumber);

	// open the file in the editor
	ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 );
}

