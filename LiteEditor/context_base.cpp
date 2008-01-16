#include "context_base.h"
#include <vector>
#include "editor_config.h"
#include "editor.h"

ContextBase::ContextBase(LEditor *container)
: m_container(container)
, m_name(wxEmptyString)
{
}

ContextBase::ContextBase(const wxString &name)
: m_name(name)
{
}

ContextBase::~ContextBase()
{
}

//provide basic indentation
void ContextBase::AutoIndent(const wxChar &ch){
	if(ch == wxT('\n')){
		//just copy the previous line indentation
		LEditor &rCtrl = GetCtrl();
		int indentSize = rCtrl.GetIndent();
		int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
		int prevLine = line - 1;
		//take the previous line indentation size
		int prevLineIndet = rCtrl.GetLineIndentation(prevLine);
		rCtrl.SetLineIndentation(line, prevLineIndet);
		//place the caret at the end of the line
		int dummy = rCtrl.GetLineIndentation(line);
		if(rCtrl.GetTabIndents()){
			dummy = dummy / indentSize;
		}
		rCtrl.SetCaretAt(rCtrl.GetCurrentPos() + dummy);
	}
}
