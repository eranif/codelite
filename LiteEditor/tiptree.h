#ifndef TIPTREE_H
#define TIPTREE_H

#include "wx/treectrl.h"
#include "debuggerobserver.h"
#include "tree_node.h"

class TipTreeData : public wxTreeItemData
{
	wxString m_displayString;
public:
	TipTreeData(const wxString &displayStr) : m_displayString(displayStr){}
	virtual ~TipTreeData(){}
};

class TipTree : public wxTreeCtrl
{
protected:
	void SortTree(std::map<void*, bool> & nodes);

public:
	TipTree(wxWindow *parent, wxWindowID id, long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
	virtual ~TipTree();

	void BuildTree(TreeNode<wxString, NodeData> *tree);
};

#endif //TIPTREE_H
