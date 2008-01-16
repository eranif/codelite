#ifndef STACK_WALKER_H
#define STACK_WALKER_H

#include "wx/string.h"
#include <wx/stackwalk.h>

#if wxUSE_STACKWALKER
class wxTextOutputStream;
class StackWalker : public wxStackWalker
{
	wxTextOutputStream *m_output;
	
public:
	StackWalker(wxTextOutputStream *output);
	virtual ~StackWalker();
	void OnStackFrame(const wxStackFrame &frame);
};
#endif 

#endif //STACK_WALKER_H
