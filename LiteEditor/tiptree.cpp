#include "tiptree.h"
#include "map"
#include "wx/settings.h"
#include "list"

TipTree::TipTree(wxWindow *parent, wxWindowID id, long style)
: wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
}

TipTree::~TipTree()
{
	DeleteAllItems();
}

void TipTree::BuildTree(TreeNode<wxString, NodeData> *tree)
{
	if(!tree){
		return;
	}
	//Walk over the tree and construct it
	Freeze();
	DeleteAllItems();
	std::map<void*, bool> m_sortItems;
	std::list< wxTreeItemId > rootChilds;
	
	//create the tree
	wxTreeItemId root = AddRoot(tree->GetData().name);
	tree->GetData().itemId = root;
	TreeWalker<wxString, NodeData> walker(tree);
	
	for(; !walker.End(); walker++)
	{
		// Add the item to the tree
		TreeNode<wxString, NodeData>* node = walker.GetNode();

		// Skip root node
		if(node->IsRoot())
			continue;
		
		wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
		if(parentHti.IsOk() == false){
			parentHti = root;
		}

		//add the item to the tree
		node->GetData().itemId = AppendItem(
												parentHti,				// parent
												node->GetData().name,	// display name
												wxNOT_FOUND,			// item image index
												wxNOT_FOUND,			// selected item image
												new TipTreeData(node->GetData().name)
											);
		m_sortItems[parentHti.m_pItem] = true;
		if(parentHti == root){
			rootChilds.push_back(node->GetData().itemId);
		}
	}
	SortTree(m_sortItems);
	
	//expand all the root direct children
	std::list<wxTreeItemId>::iterator iter = rootChilds.begin();
	for(; iter != rootChilds.end(); iter++){
		wxTreeItemId item = (*iter);
		if(item.IsOk() && ItemHasChildren(item)){
			Expand(item);
		}
	}
	delete tree;
	Thaw();
}

void TipTree::SortTree(std::map<void*, bool> & nodes)
{
	std::map<void*, bool>::iterator iter = nodes.begin();
	for(; iter != nodes.end(); iter++){
		wxTreeItemId item = iter->first;
		if(item.IsOk()){
			// Does this node has children?
			if( GetChildrenCount( item ) == 0 )
				continue;
			SortChildren(item); 
		}
	}
}


