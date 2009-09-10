#include "debuggerasciiviewer.h"
#include "manager.h"
#include "debugger.h"

DebuggerAsciiViewer::DebuggerAsciiViewer( wxWindow* parent )
		: DebuggerAsciiViewerBase( parent )
		, m_debugger(NULL)
{
	wxFont font(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	// hide all margins
	m_textView->SetMarginWidth(0, 0);
	m_textView->SetMarginWidth(1, 0);
	m_textView->SetMarginWidth(2, 0);
	m_textView->SetMarginWidth(3, 0);
	m_textView->SetMarginWidth(4, 0);
	
	// Set wrap mode on
	m_textView->SetWrapMode(wxSCI_WRAP_WORD);
	// Use NULL lexer
	m_textView->SetLexer(wxSCI_LEX_NULL);
	
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
	// Evaluate the tip
	wxString evaluated;
	m_debugger->GetTip(m_textCtrlExpression->GetValue(), evaluated );

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
