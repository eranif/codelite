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
	if (!value) {
		value = 32;
	}
	return value;
}

void MemoryView::SetViewString(const wxString& text)
{
	m_textCtrlMemory->Freeze();
	// get old value
	wxString oldValue = m_textCtrlMemory->GetValue();

	m_textCtrlMemory->Clear();


	// first check that we are trying to view the same addresses
	wxString newAddr = text.BeforeFirst(wxT(':'));
	wxString oldAddr = oldValue.BeforeFirst(wxT(':'));

	// set the new value
	m_textCtrlMemory->SetValue(text);

	if (newAddr == oldAddr) {

		size_t shortLen = text.Length() < oldValue.Length() ? text.Length() : oldValue.Length();

		// same address, loop over the strings
		wxTextAttr style;
		style.SetTextColour(wxT("RED"));

		long start(wxNOT_FOUND);
		bool needColouring(false);
		long possibleStart(0);

		for (size_t i=0; i<shortLen; i++) {

			// reset word
			if ((text.GetChar(i) == wxT(' ') || text.GetChar(i) == wxT('\n')) && needColouring) {

				if (text.GetChar(i) == wxT('\n')) {
					// the last word read was the ascii part of the memory view
					// read backward until we find a space
					int xx(i);
					for(; xx>0; xx--){
						
						if(text.GetChar(xx) == wxT(' ')){
							xx++;
							break;
						}
					}
					
					for(; xx<(int)shortLen; xx++){
						if(text.GetChar(xx) == wxT('\n')){
							break;
						}
						
						if(text.GetChar(xx) != oldValue.GetChar(xx)){
							m_textCtrlMemory->SetStyle(xx, xx+1, style);
						}
					}
					
				} else {
					
					m_textCtrlMemory->SetStyle(start, (long)i, style);
				}
				
				start = wxNOT_FOUND;
				needColouring = false;
			}

			if (text.GetChar(i) == wxT(' ') || text.GetChar(i) == wxT('\n')) {
				possibleStart = (long)i;
			}

			if (text.GetChar(i) != oldValue.GetChar(i)) {
				needColouring = true;
				if (start == wxNOT_FOUND) {
					start = possibleStart;
				}
			}
		}
	}

	m_textCtrlMemory->Thaw();
}
