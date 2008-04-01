#include "replaceinfilespanel.h"
#include "wx/progdlg.h"
#include "cl_editor.h"
#include "manager.h"

ReplaceInFilesPanel::ReplaceInFilesPanel( wxWindow* parent )
		:
		ReplaceInFilesBasePanel( parent )
{

}

void ReplaceInFilesPanel::OnItemSelected( wxCommandEvent& event )
{
	int selection;
	selection = m_listBox1->GetSelection();
	if (selection == wxNOT_FOUND) {
		return;
	}

	long lineNumber = -1;
	long matchLen = wxNOT_FOUND;
	long col = wxNOT_FOUND;
	wxString fileName;
	wxString pattern;

	ParseEntry(selection, lineNumber, col, matchLen, fileName, pattern);

	// open the file in the editor
	if (ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 )) {
		LEditor *editor = ManagerST::Get()->GetActiveEditor();
		if (editor) {
			if (col >= 0 && matchLen >= 0) {
				int offset = editor->PositionFromLine(lineNumber-1);
				editor->SetSelection(offset + col, offset + col + matchLen);
			}
		}
	}
	m_listBox1->SetFocus();
}

void ReplaceInFilesPanel::OnItemDClicked( wxCommandEvent& event )
{
	OnItemSelected(event);
	DoReplaceSelection();
	m_listBox1->SetFocus();

	int sel = m_listBox1->GetSelection();
	if (sel != wxNOT_FOUND) {
		int nextItem(wxNOT_FOUND);
		if ((size_t)sel < m_listBox1->GetCount()-1 && m_listBox1->GetCount() > 1) {
			nextItem = sel;
		} else if ((size_t)sel == m_listBox1->GetCount()-1 && m_listBox1->GetCount() > 1) {
			nextItem = sel - 1;
		}

		if (nextItem != wxNOT_FOUND) {
			long line;
			long matchLen;
			long col;
			wxString fileName;
			wxString pattern;

			ParseEntry(sel, line, col, matchLen, fileName, pattern);
			AdjustItems((unsigned int)sel, m_textCtrlReplaceWith->GetValue().Length()-matchLen, fileName, line);
		}

		m_listBox1->Delete((unsigned int)sel);
		m_listBox1->Select((unsigned int)nextItem);

		//select next item
		OnItemSelected(event);
	}
}

void ReplaceInFilesPanel::OnReplaceAll( wxCommandEvent& event )
{
	//parse all entries in the list, collect the file names and perform a massive replace
	size_t count = m_listBox1->GetCount();

	// Create a progress dialog
	wxProgressDialog prgDlg(wxT("Performing replace all..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)count, NULL, wxPD_APP_MODAL | wxPD_SMOOTH|wxPD_AUTO_HIDE);
	prgDlg.GetSizer()->Fit( &prgDlg );
	prgDlg.Layout();
	prgDlg.Centre();

	wxString msg;
	prgDlg.Update(0, wxT("Replacing..."));
	for (size_t i=0; i< m_listBox1->GetCount(); i++) {
		
		long cur_line;
		long matchLen;
		long col;
		wxString file_name;
		wxString pattern;

		ParseEntry(i, cur_line, col, matchLen, file_name, pattern);
		
		//open this file
		if( ManagerST::Get()->OpenFile(file_name, wxEmptyString) ) {
			//do the actual replacement here
			//TODO:: DO IT!
		}
		
		// update the progress bar
		msg.Clear();
		msg << wxT("Replacing in file: ") << file_name;
		prgDlg.Update(i, msg);
	}
}

void ReplaceInFilesPanel::OnReplaceAllUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBox1->IsEmpty() == false);
}

void ReplaceInFilesPanel::AddResults(SearchResultList *res)
{
	SearchResultList::iterator iter = res->begin();

	m_listBox1->Freeze();

	wxString msg;
	wxArrayString arr;
	for (; iter != res->end(); iter++) {
		msg = (*iter).GetMessage();
		msg = msg.Trim().Trim(false);
		arr.Add( msg );
	}

	m_listBox1->Append(arr);
	m_listBox1->Thaw();
	delete res;
	m_listBox1->SetFocus();
}

void ReplaceInFilesPanel::Clear()
{
	m_listBox1->Clear();
}

void ReplaceInFilesPanel::DoReplaceSelection()
{
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	if (editor) {
		if (editor->GetSelectedText().IsEmpty() == false) {
			editor->ReplaceSelection(m_textCtrlReplaceWith->GetValue());
		}
	}
}

void ReplaceInFilesPanel::OnClearResults(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Clear();
}

void ReplaceInFilesPanel::OnClearResultsUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBox1->IsEmpty() == false);
}

void ReplaceInFilesPanel::ParseEntry(int index, long &line, long &col, long &matchLen, wxString &fileName, wxString &pattern)
{
	wxString lineText = m_listBox1->GetString((unsigned int)index);

	// each line has the format of
	// file(line, col, len): text
	fileName = lineText.BeforeFirst(wxT('('));
	wxString strLineNumber = lineText.AfterFirst(wxT('('));
	wxString strLen = strLineNumber.AfterFirst(wxT(',')).AfterFirst(wxT(',')).BeforeFirst(wxT(')'));
	wxString strCol = strLineNumber.AfterFirst(wxT(',')).BeforeFirst(wxT(','));

	pattern = lineText.AfterFirst(wxT('('));
	pattern = pattern.AfterFirst(wxT(' '));

	strLineNumber = strLineNumber.BeforeFirst(wxT(','));
	strLineNumber = strLineNumber.Trim();
	strLineNumber.ToLong(&line);

	strLen = strLen.Trim().Trim(false);
	strLen.ToLong( &matchLen );

	strCol = strCol.Trim().Trim(false);
	strCol.ToLong( &col );
}

void ReplaceInFilesPanel::AdjustItems(unsigned int from, int diff, const wxString &fileName, long line)
{
	for (unsigned int i=from; i< m_listBox1->GetCount(); i++) {
		long cur_line;
		long matchLen;
		long col;
		wxString file_name;
		wxString pattern;

		ParseEntry(i, cur_line, col, matchLen, file_name, pattern);
		if (file_name == fileName && line == cur_line) {
			//adjust this item
			wxString msg;
			msg << file_name
			<< wxT("(")
			<< line
			<< wxT(",")
			<< col + diff
			<< wxT(",")
			<< matchLen
			<< wxT("): ")
			<< pattern;
			m_listBox1->SetString(i, msg);
		} else {
			break;
		}
	}
}
