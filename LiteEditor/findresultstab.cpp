#include "findresultstab.h"
#include "wx/string.h"
#include "manager.h"

FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name)
: OutputTabWindow(parent, id, name)
{
}

FindResultsTab::~FindResultsTab()
{
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

