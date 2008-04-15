#include "addincludefiledlg.h"
#include "wx/filename.h"
#include "wx/regex.h"

wxArrayString AddIncludeFileDlg::m_includePath;

AddIncludeFileDlg::AddIncludeFileDlg( wxWindow* parent, const wxString &fullpath, const wxString &text, int lineNo )
		:
		AddIncludeFileDlgBase( parent )
		, m_fullpath(fullpath)
		, m_text(text)
		, m_line(lineNo)
{
	UpdateLineToAdd();
	
	//initialise the preview window
	//---------------------------------------------------------
	wxFont font(10, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	m_textCtrlPreview->MarkerDefine(0x7, wxSCI_MARK_ARROW);
	m_textCtrlPreview->MarkerSetBackground(0x7, wxT("YELLOW GREEN"));
	m_textCtrlPreview->StyleSetFont(0, font);
	

	//set the initial text
	m_textCtrlPreview->SetReadOnly(false);
	m_textCtrlPreview->AddText(m_text);
	m_textCtrlPreview->EmptyUndoBuffer();
	SetAndMarkLine();
	
	m_textCtrlLineToAdd->SetFocus();
}

void AddIncludeFileDlg::UpdateLineToAdd()
{
	wxString line;
	wxFileName fn(m_fullpath);
	m_textCtrlFullPath->SetValue(fn.GetFullPath());

	//try to get a match in the include path for this file
	wxString pp = fn.GetFullPath();
	pp.Replace(wxT("\\"), wxT("/"));
	
	wxString rest;
	for(size_t i=0; i< m_includePath.GetCount(); i++){
		if(pp.StartsWith( m_includePath.Item(i) , &rest )){
			break;
		}
	}
	
	if(rest.IsEmpty()){
		rest = fn.GetFullName();
	}
	
	line << wxT("#include \"") << rest << wxT("\"");
	m_textCtrlLineToAdd->SetValue(line);

}

void AddIncludeFileDlg::SetAndMarkLine()
{
	wxString line = m_textCtrlLineToAdd->GetValue();
	m_textCtrlPreview->SetReadOnly(false);
	if (m_textCtrlPreview->CanUndo()) {
		m_textCtrlPreview->Undo();
	}

	m_textCtrlPreview->BeginUndoAction();
	if (m_line == wxNOT_FOUND) {
		m_line = 0;
	}
	m_textCtrlPreview->MarkerDeleteAll(0x7);
	long pos = m_textCtrlPreview->PositionFromLine(m_line);
	m_textCtrlPreview->InsertText(pos, line + wxT("\n"));
	m_textCtrlPreview->MarkerAdd(m_line, 0x7);
	m_textCtrlPreview->SetCurrentPos(pos);
	m_textCtrlPreview->SetSelectionStart(pos);
	m_textCtrlPreview->SetSelectionEnd(pos);
	m_textCtrlPreview->EnsureCaretVisible();
	m_textCtrlPreview->EndUndoAction();
	m_textCtrlPreview->SetReadOnly(true);
}

void AddIncludeFileDlg::OnTextUpdated(wxCommandEvent &e)
{
	wxUnusedVar(e);
	SetAndMarkLine();
}

void AddIncludeFileDlg::OnButtonDown(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_line+2 >= m_textCtrlPreview->GetLineCount()) {
		return;
	}
	m_line++;
	SetAndMarkLine();
}

void AddIncludeFileDlg::OnButtonUp(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_line-1 < 0) {
		return;
	}
	m_line--;
	SetAndMarkLine();
}

void AddIncludeFileDlg::OnButtonOK(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//get the include file to add
	wxString fullpath = m_textCtrlFullPath->GetValue();
	static wxRegEx reIncludeFile(wxT("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.]*)"));
	wxString relativePath;

	if (reIncludeFile.Matches(m_textCtrlLineToAdd->GetValue())) {
		relativePath = reIncludeFile.GetMatch(m_textCtrlLineToAdd->GetValue(), 1);
	}

	fullpath.Replace(wxT("\\"), wxT("/"));
	relativePath.Replace(wxT("\\"), wxT("/"));
	wxFileName fn(fullpath);
	
	wxString inclPath;
	if (fullpath.EndsWith(relativePath, &inclPath) && 
		fullpath != relativePath &&	//dont save the '.' path this is done by default
		fn.GetFullName() != relativePath)  //if the relative path is only file name, nothing to cache
	{
		m_includePath.Add(inclPath);
	}
	EndModal(wxID_OK);
}

void AddIncludeFileDlg::OnClearCachedPaths(wxCommandEvent &e)
{
	wxUnusedVar(e);
	m_includePath.Clear();
	UpdateLineToAdd();
	SetAndMarkLine();
}


