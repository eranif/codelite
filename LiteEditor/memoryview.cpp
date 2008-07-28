#include "manager.h"
#include "memoryview.h"

MemoryView::MemoryView( wxWindow* parent )
		:
		MemoryViewBase( parent )
{

}

void MemoryView::OnTextEntered( wxCommandEvent& event )
{
	wxUnusedVar(event);
}

void MemoryView::OnEvaluate( wxCommandEvent& event )
{
	wxUnusedVar(event);
	ManagerST::Get()->UpdateDebuggerPane();
}

void MemoryView::OnEvaluateUI( wxUpdateUIEvent& event )
{
	event.Enable(m_textCtrlExpression->GetValue().IsEmpty() == false);
}

void MemoryView::Clear()
{
	m_textCtrlExpression->Clear();
	m_textCtrlMemory->Clear();
}

size_t MemoryView::GetSize() const
{
	long value(0);
	m_textCtrlSize->GetValue().ToLong(&value);
	
	// set default memory size to 32 bytes
	if(!value) {
		value = 32;
	}
	return value;
}

void MemoryView::SetViewString(const wxString& text)
{
	m_textCtrlMemory->Clear();
	m_textCtrlMemory->SetValue(text);
}
