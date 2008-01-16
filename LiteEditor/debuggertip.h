#ifndef DEBUGGERTIP_H
#define DEBUGGERTIP_H

#if 0
#include "wx/popupwin.h"
#include "tree_node.h"
#include "debuggerobserver.h"

class TipTree;

class DebuggerTip : public wxPopupWindow
{
	TipTree *m_localVarsTree;
protected:
	void OnTreeSized(wxCommandEvent &event);
	
public:
	DebuggerTip(wxWindow *parent, const wxString &expression, TreeNode<wxString, NodeData> *tree, long pos);
	~DebuggerTip();
	
	void Initialize(const wxString &expression, TreeNode<wxString, NodeData> *tree, long pos);
};

#endif //DEBUGGERTIP_H


#endif

