#include "stack_walker.h"
#include "wx/txtstrm.h"

#if wxUSE_STACKWALKER

StackWalker::StackWalker(wxTextOutputStream *output)
: m_output(output)
{
}

StackWalker::~StackWalker()
{
}

void StackWalker::OnStackFrame(const wxStackFrame &frame)
{
	*m_output 
		<< frame.GetModule() << wxT(" ")
		<< frame.GetName() << wxT(" ")
		<< frame.GetFileName() << wxT(" ")
		<< (int)frame.GetLine() << wxT("\n");
}
#endif 



