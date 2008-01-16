#ifndef LOCALVARSTREE_H
#define LOCALVARSTREE_H

#include "wx/treectrl.h"
#include "debuggerobserver.h"
#include "tree_node.h"

class LocalVarData : public wxTreeItemData
{
	wxString m_displayString;
public:
	LocalVarData(const wxString &displayStr):m_displayString(displayStr){}
	virtual ~LocalVarData(){}
};

class LocalVarsTree : public wxTreeCtrl
{
	TreeNode<wxString, NodeData> *m_tree;
	std::map<void*, bool> m_sortItems;

protected:
	void SortTree(std::map<void*, bool> & nodes);

public:
	LocalVarsTree(wxWindow *parent, wxWindowID id, long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
	virtual ~LocalVarsTree();

	void BuildTree(TreeNode<wxString, NodeData> *tree);
	TreeNode<wxString, NodeData> *GetTree() {return m_tree;}
	void Clear();
};

#endif //LOCALVARSTREE_H


