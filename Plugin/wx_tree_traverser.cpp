#include "wx_tree_traverser.h"

wxTreeTraverser::wxTreeTraverser(wxTreeCtrl* tree)
	: m_tree(tree)
{
}

wxTreeTraverser::~wxTreeTraverser()
{
}

void wxTreeTraverser::DoTraverse(const wxTreeItemId& item)
{
	if(item.IsOk() == false)
		return;
	
	// Call the user callback
	OnItem(item);
	
	// Recurse the children
	if(m_tree->ItemHasChildren(item)) {
		wxTreeItemIdValue cookie;
		wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
		while ( child.IsOk() ) {
			DoTraverse(child);
			child = m_tree->GetNextChild(item, cookie);
		}
	}
}

void wxTreeTraverser::OnItem(const wxTreeItemId& item)
{
}

void wxTreeTraverser::Traverse(const wxTreeItemId& item)
{
	DoTraverse(item.IsOk() ? item : m_tree->GetRootItem());
}

