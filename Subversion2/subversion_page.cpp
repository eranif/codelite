#include "subversion_page.h"
#include <wx/xrc/xmlres.h>
#include "svntreedata.h"
#include <wx/imaglist.h>

SubversionPage::SubversionPage( wxWindow* parent )
		: SubversionPageBase( parent )
{
	CreatGUIControls();
}

void SubversionPage::OnChangeRootDir( wxCommandEvent& event )
{

}

void SubversionPage::OnTreeMenu( wxTreeEvent& event )
{
	// TODO: Implement OnTreeMenu
}

void SubversionPage::CreatGUIControls()
{
	BuildTree();
}

void SubversionPage::BuildTree()
{
	m_treeCtrl->DeleteAllItems();
	wxTreeItemId root = m_treeCtrl->AddRoot(wxT("Root"));
	
	wxImageList *imageList = new wxImageList(16, 16, true);
	
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("error")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("project_conflict")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("files_unversioned")));
	
	m_treeCtrl->AssignImageList( imageList );
	
	// Add the basic four root items
	m_treeCtrl->AppendItem(root, wxT("Modified Files"), 0, 0, new SvnTreeData(SvnTreeData::SvnNodeTypeModifiedRoot));
	m_treeCtrl->AppendItem(root, wxT("Conflicted Files"), 1, 1, new SvnTreeData(SvnTreeData::SvnNodeTypeConflictRoot));
	m_treeCtrl->AppendItem(root, wxT("Unversioned Files"), 2, 2, new SvnTreeData(SvnTreeData::SvnNodeTypeUnversionedRoot));
}
