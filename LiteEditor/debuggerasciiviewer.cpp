#include "debuggerasciiviewer.h"
#include "frame.h"
#include "debuggerpane.h"
#include "manager.h"
#include "debugger.h"

static void sDefineMarker(wxScintilla *s, int marker, int markerType, wxColor fore, wxColor back)
{
	s->MarkerDefine(marker, markerType);
	s->MarkerSetForeground(marker, fore);
	s->MarkerSetBackground(marker, back);
}

DebuggerAsciiViewer::DebuggerAsciiViewer( wxWindow* parent )
		: DebuggerAsciiViewerBase( parent )
		, m_debugger(NULL)
		, m_dbgCommand(wxT("print"))
{
	wxFont font(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	// hide all margins
	m_textView->SetMarginWidth(0, 0);
	m_textView->SetMarginWidth(1, 0);
	m_textView->SetMarginWidth(2, 0);
	m_textView->SetMarginWidth(3, 0);
	m_textView->SetMarginWidth(4, 0);

	m_textView->SetMarginSensitive(4, true);
	m_textView->SetProperty(wxT("fold"), wxT("1"));

	sDefineMarker(m_textView, wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	sDefineMarker(m_textView, wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	sDefineMarker(m_textView, wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	sDefineMarker(m_textView, wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	sDefineMarker(m_textView, wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	sDefineMarker(m_textView, wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	sDefineMarker(m_textView, wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	// set wrapped line indicator
	m_textView->SetWrapVisualFlags(1);
	m_textView->SetWrapStartIndent(4);
	m_textView->SetScrollWidthTracking(true);
	m_textView->StyleSetForeground(wxSCI_STYLE_DEFAULT, wxT("GREY"));

	// Set wrap mode on
	m_textView->SetWrapMode(wxSCI_WRAP_WORD);
	// Use NULL lexer
	m_textView->SetLexer(wxSCI_LEX_CPP);
	m_textView->SetMarginMask(4, wxSCI_MASK_FOLDERS);

	// Set TELETYPE font (monospace)
	m_textView->StyleSetFont(wxSCI_STYLE_DEFAULT, font);
	m_textView->StyleSetSize(wxSCI_STYLE_DEFAULT, 8   );

	m_textView->SetReadOnly(true);
}

void DebuggerAsciiViewer::OnEnter( wxCommandEvent& event )
{
	if ( m_textCtrlExpression->GetValue().IsEmpty() ) {

		m_textView->SetReadOnly(false);
		m_textView->ClearAll();
		m_textView->SetReadOnly(true);

	} else if ( m_debugger && m_debugger->IsRunning() && ManagerST::Get()->DbgCanInteract() ) {
		DoUpdateView();

	}
}

void DebuggerAsciiViewer::SetExpression(const wxString& expr)
{
	m_textCtrlExpression->SetValue(expr);
	DoUpdateView();
}

void DebuggerAsciiViewer::SetDebugger(IDebugger* debugger)
{
	m_debugger = debugger;
}

void DebuggerAsciiViewer::DoUpdateView()
{
	Manager *     m = ManagerST::Get();
	DebuggerPane *pane = Frame::Get()->GetDebuggerPane();
	bool          visible = ( ( m->IsPaneVisible ( wxT ( "Debugger" ) ) && pane->GetNotebook()->GetCurrentPage() == ( wxWindow* )this ) || m->IsPaneVisible ( DebuggerPane::ASCII_VIEWER ) );
	bool          can_interact = m_debugger && m_debugger->IsRunning() && m->DbgCanInteract();

	if ( !visible ) {
		return;
	}

	// Evaluate the tip
	wxString evaluated (wxEmptyString);
	wxString expression ( m_textCtrlExpression->GetValue() );
	expression.Trim().Trim(false);

	if ( expression.IsEmpty() ) {
		evaluated = wxT("");
	} else if ( can_interact ) {
		m_debugger->GetTip(m_dbgCommand, m_textCtrlExpression->GetValue(), evaluated );
	}

	evaluated.Replace(wxT("\r\n"), wxT("\n"));
	evaluated.Replace(wxT("\n,"), wxT(",\n"));
	evaluated.Replace(wxT("\n\n"), wxT("\n"));

	// update the view
	m_textView->SetReadOnly(false);
	m_textView->ClearAll();
	m_textView->SetText(evaluated);
	m_textView->SetReadOnly(true);
}

void DebuggerAsciiViewer::UpdateView()
{
	DoUpdateView();
}

void DebuggerAsciiViewer::SetDbgCommand(const wxString& dbgCmd)
{
	m_textCtrlDbgCommand->SetValue(dbgCmd);
	m_dbgCommand = dbgCmd;
}
