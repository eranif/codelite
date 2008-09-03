#include "workspace.h"
#include <wx/xrc/xmlres.h>
#include <deque>
#include "virtualdirectoryselector.h"
#include "tree_node.h"

VirtualDirectorySelector::VirtualDirectorySelector( wxWindow* parent, Workspace *wsp)
		: VirtualDirectorySelectorBase( parent )
		, m_workspace(wsp)
{
	m_treeCtrl->SetFocus();
	DoBuildTree();
}

void VirtualDirectorySelector::OnItemSelected( wxTreeEvent& event )
{
	m_staticTextPreview->SetLabel(DoGetPath(event.GetItem()));
}

void VirtualDirectorySelector::OnButtonOK( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

void VirtualDirectorySelector::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

wxString VirtualDirectorySelector::DoGetPath(const wxTreeItemId& item)
{
	if (!item.IsOk()) {
		return wxEmptyString;
	}

	if(m_treeCtrl->GetItemImage(item) != 2){ // not a virtual folder
		return wxEmptyString;
	}

	std::deque<wxString> queue;
	wxString text = m_treeCtrl->GetItemText( item );
	queue.push_front( text );

	wxTreeItemId p = m_treeCtrl->GetItemParent( item );
	while ( p.IsOk() && p != m_treeCtrl->GetRootItem() ) {

		text = m_treeCtrl->GetItemText( p );
		queue.push_front( text );
		p = m_treeCtrl->GetItemParent( p );
	}

	wxString path;
	size_t count = queue.size();
	for ( size_t i=0; i<count; i++ ) {
		path += queue.front();
		path += wxT( ":" );
		queue.pop_front();
	}

	if ( !queue.empty() ) {
		path += queue.front();
	} else {
		path = path.BeforeLast( wxT( ':' ) );
	}

	return path;
}

void VirtualDirectorySelector::DoBuildTree()
{
	wxImageList *images = new wxImageList(16, 16);
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "workspace" ) ) );	//0
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "project" ) ) );	//1
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "folder" ) ) );		//2
	m_treeCtrl->AssignImageList(images);

	if (m_workspace) {
		wxArrayString projects;
		m_workspace->GetProjectList(projects);
		
		VisualWorkspaceNode nodeData;
		nodeData.name = m_workspace->GetName();
		nodeData.type = ProjectItem::TypeWorkspace;
		
		TreeNode<wxString, VisualWorkspaceNode> *tree = new TreeNode<wxString, VisualWorkspaceNode>(m_workspace->GetName(), nodeData);

		for (size_t i=0; i<projects.GetCount(); i++) {
			wxString err;
			ProjectPtr p = m_workspace->FindProjectByName(projects.Item(i), err);
			if (p) {
				p->GetVirtualDirectories(tree);
			}
		}
		
		//create the tree
		wxTreeItemId root = m_treeCtrl->AddRoot(nodeData.name, 0, 0);
		tree->GetData().itemId = root;
		TreeWalker<wxString, VisualWorkspaceNode> walker(tree);

		for (; !walker.End(); walker++) {
			// Add the item to the tree
			TreeNode<wxString, VisualWorkspaceNode>* node = walker.GetNode();

			// Skip root node
			if (node->IsRoot())
				continue;

			wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
			if (parentHti.IsOk() == false) {
				parentHti = root;
			}

			int imgId(2); // Virtual folder
			switch(node->GetData().type){
			case ProjectItem::TypeWorkspace:
				imgId = 0;
				break;
			case ProjectItem::TypeProject:
				imgId = 1;
				break;
			case ProjectItem::TypeVirtualDirectory:
			default:
				imgId = 2;
				break;
			
			}
			
			//add the item to the tree
			node->GetData().itemId = m_treeCtrl->AppendItem(
			                             parentHti,				// parent
			                             node->GetData().name,	// display name
			                             imgId,					// item image index
			                             imgId					// selected item image
			                         );
		}

		if (root.IsOk() && m_treeCtrl->HasChildren(root)) {
			m_treeCtrl->Expand(root);
		}
		delete tree;
	}
}

void VirtualDirectorySelector::OnButtonOkUI(wxUpdateUIEvent& event)
{
	wxTreeItemId id = m_treeCtrl->GetSelection();
	event.Enable(id.IsOk() && m_treeCtrl->GetItemImage(id) == 2);
}
