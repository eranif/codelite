//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : VirtualDirectorySelectorDlg.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "VirtualDirectorySelectorDlg.h"
#include "bitmap_loader.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "plugin.h"
#include "project.h"
#include "tree_node.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include <deque>
#include <wx/imaglist.h>
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

class MyVdTreeItemData : public wxTreeItemData
{
    int m_itemType = ProjectItem::TypeInvalid;

public:
    MyVdTreeItemData(int type)
        : m_itemType(type)
    {
    }
    virtual ~MyVdTreeItemData() {}

    bool IsFolder() const { return m_itemType == ProjectItem::TypeVirtualDirectory; }
    bool IsProject() const { return m_itemType == ProjectItem::TypeProject; }
};

VirtualDirectorySelectorDlg::VirtualDirectorySelectorDlg(wxWindow* parent, clCxxWorkspace* wsp,
                                                         const wxString& initialPath, const wxString& projectname)
    : VirtualDirectorySelectorDlgBaseClass(parent)
    , m_workspace(wsp)
    , m_projectName(projectname)
    , m_initialPath(initialPath)
    , m_reloadTreeNeeded(false)
{
    m_treeCtrl->SetFocus();
    DoBuildTree();
    GetSizer()->Fit(this);
    CentreOnParent();
}

VirtualDirectorySelectorDlg::~VirtualDirectorySelectorDlg() {}

void VirtualDirectorySelectorDlg::OnItemSelected(wxTreeEvent& event)
{
    m_staticTextPreview->SetLabel(DoGetPath(m_treeCtrl, event.GetItem(), true));
}

void VirtualDirectorySelectorDlg::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);

    if(m_reloadTreeNeeded) {
        m_reloadTreeNeeded = false;
        wxCommandEvent buildTree(wxEVT_REBUILD_WORKSPACE_TREE);
        EventNotifier::Get()->AddPendingEvent(buildTree);
    }
}

void VirtualDirectorySelectorDlg::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);

    // Even though the user cancelled,  we still need to reload if a new VDir was created
    if(m_reloadTreeNeeded) {
        m_reloadTreeNeeded = false;
        wxCommandEvent buildTree(wxEVT_REBUILD_WORKSPACE_TREE);
        EventNotifier::Get()->AddPendingEvent(buildTree);
    }
}

wxString VirtualDirectorySelectorDlg::DoGetPath(clTreeCtrl* tree, const wxTreeItemId& item, bool validateFolder)
{
    if(!item.IsOk()) { return wxEmptyString; }
    if(validateFolder) {
        MyVdTreeItemData* cd = dynamic_cast<MyVdTreeItemData*>(tree->GetItemData(item));
        if(!(cd && cd->IsFolder())) { return ""; }
    }

    std::deque<wxString> queue;
    wxString text = tree->GetItemText(item);
    queue.push_front(text);

    wxTreeItemId p = tree->GetItemParent(item);
    while(true) {

        if(!p.IsOk() || p == tree->GetRootItem()) break;

        FilewViewTreeItemData* data = dynamic_cast<FilewViewTreeItemData*>(tree->GetItemData(p));
        if(data && (data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder)) {
            // We reached the top level
            break;
        }

        text = tree->GetItemText(p);
        queue.push_front(text);
        p = tree->GetItemParent(p);
    }

    wxString path;
    size_t count = queue.size();
    for(size_t i = 0; i < count; i++) {
        path += queue.front();
        path += wxT(":");
        queue.pop_front();
    }

    if(!queue.empty()) {
        path += queue.front();
    } else {
        path = path.BeforeLast(wxT(':'));
    }

    return path;
}
void VirtualDirectorySelectorDlg::DoBuildTree()
{
    m_treeCtrl->DeleteAllItems();
    m_treeCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());

    int workspaceImgId = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeWorkspace);
    int folderImgId = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
    int folderImgIdExpanded = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderExpanded);
    int projectImgId = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeProject);
    int projectImgIdExpanded = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeProjectExpanded);

    if(m_workspace) {
        wxArrayString projects;
        m_workspace->GetProjectList(projects);

        VisualWorkspaceNode nodeData;
        nodeData.name = m_workspace->GetName();
        nodeData.type = ProjectItem::TypeWorkspace;

        TreeNode<wxString, VisualWorkspaceNode>* tree =
            new TreeNode<wxString, VisualWorkspaceNode>(m_workspace->GetName(), nodeData);

        for(size_t i = 0; i < projects.GetCount(); i++) {
            if(!m_projectName.empty() && projects.Item(i) != m_projectName) {
                // If we were passed a specific project, display only that one
                continue;
            }

            wxString err;
            ProjectPtr p = m_workspace->FindProjectByName(projects.Item(i), err);
            if(p) { p->GetVirtualDirectories(tree); }
        }

        // create the tree
        wxTreeItemId root = m_treeCtrl->AddRoot(nodeData.name, workspaceImgId, workspaceImgId,
                                                new MyVdTreeItemData(ProjectItem::TypeWorkspace));
        tree->GetData().itemId = root;
        TreeWalker<wxString, VisualWorkspaceNode> walker(tree);

        for(; !walker.End(); walker++) {
            // Add the item to the tree
            TreeNode<wxString, VisualWorkspaceNode>* node = walker.GetNode();

            // Skip root node
            if(node->IsRoot()) continue;

            wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
            if(parentHti.IsOk() == false) { parentHti = root; }

            int imgId; // Virtual folder
            int imgIdExpanded;
            switch(node->GetData().type) {
            case ProjectItem::TypeWorkspace:
                imgId = 0;
                imgIdExpanded = 0;
                break;
            case ProjectItem::TypeProject:
                imgId = projectImgId;
                imgIdExpanded = projectImgIdExpanded;
                break;
            case ProjectItem::TypeVirtualDirectory:
            default:
                imgId = folderImgId;
                imgIdExpanded = folderImgIdExpanded;
                break;
            }

            // add the item to the tree
            node->GetData().itemId = m_treeCtrl->AppendItem(parentHti,            // parent
                                                            node->GetData().name, // display name
                                                            imgId,                // item image index
                                                            imgIdExpanded,        // selected item image
                                                            new MyVdTreeItemData(node->GetData().type));
        }
        if(root.IsOk() && m_treeCtrl->ItemHasChildren(root)) { m_treeCtrl->Expand(root); }
        wxDELETE(tree);
    }
    // if a initialPath was provided, try to find and select it
    if(SelectPath(m_initialPath)) { m_treeCtrl->Expand(m_treeCtrl->GetSelection()); }
}

void VirtualDirectorySelectorDlg::OnButtonOkUI(wxUpdateUIEvent& event)
{
    wxTreeItemId id = m_treeCtrl->GetSelection();
    if(!id.IsOk()) {
        event.Enable(false);
    } else {
        MyVdTreeItemData* cd = dynamic_cast<MyVdTreeItemData*>(m_treeCtrl->GetItemData(id));
        event.Enable(cd->IsFolder());
    }
}

wxTreeItemId VirtualDirectorySelectorDlg::FindItemForPath(const wxString& path)
{
    if(path.empty()) { return wxTreeItemId(); }

    wxArrayString tokens = wxStringTokenize(path, wxT(":"), wxTOKEN_STRTOK);
    wxTreeItemId item = m_treeCtrl->GetRootItem();
    if(m_treeCtrl->GetWindowStyle() & wxTR_HIDE_ROOT) {
        if(!item.IsOk() || !m_treeCtrl->HasChildren(item)) { return wxTreeItemId(); }
    }
    // We need to pump-prime with the first token, otherwise the loop is never entered
    wxTreeItemIdValue cookie;
    item = m_treeCtrl->GetFirstChild(item, cookie);
    if(m_treeCtrl->GetItemText(item) == path) {
        return item; // Root's first child, the top VDir, was the item to find
    }

    for(size_t i = 1; i < tokens.GetCount(); ++i) {
        if(item.IsOk() && m_treeCtrl->HasChildren(item)) {
            // loop over the children of this node, and search for a match
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_treeCtrl->GetFirstChild(item, cookie);
            while(child.IsOk()) {
                if(m_treeCtrl->GetItemText(child) == tokens.Item(i)) {
                    item = child;
                    if(i + 1 == tokens.GetCount()) {
                        return item; // Success!
                    }
                    break;
                }
                child = m_treeCtrl->GetNextChild(item, cookie);
            }
        }
    }

    return wxTreeItemId();
}

bool VirtualDirectorySelectorDlg::SelectPath(const wxString& path)
{
    wxTreeItemId item = FindItemForPath(path);
    if(!item.IsOk()) {
        // No match, so try to find a sensible default
        // Start with the root, but this will fail for a hidden root...
        item = m_treeCtrl->GetRootItem();
        if(m_treeCtrl->GetWindowStyle() & wxTR_HIDE_ROOT) {
            if(!item.IsOk() || !m_treeCtrl->HasChildren(item)) { return false; }

            wxTreeItemIdValue cookie;
            item = m_treeCtrl->GetFirstChild(item, cookie);
        }
    }

    if(item.IsOk()) {
        m_treeCtrl->EnsureVisible(item);
        m_treeCtrl->SelectItem(item);
        return true;
    }

    return false;
}

void VirtualDirectorySelectorDlg::SetText(const wxString& text) { m_staticText1->SetLabel(text); }

void VirtualDirectorySelectorDlg::OnNewVD(wxCommandEvent& event)
{
    static int counter = 0;
    wxTreeItemId id = m_treeCtrl->GetSelection();
    if(id.IsOk() == false) return;

    wxString curpath = DoGetPath(m_treeCtrl, id, false);
    wxTreeItemId item = FindItemForPath(m_projectName + ':' + m_suggestedName);
    wxString name;

    if(!item.IsOk()) { // We don't want to suggest an already-existing path!
        name = m_suggestedName;
        wxString rest;
        if(name.StartsWith(curpath.AfterFirst(':'), &rest)) {
            name = rest;
            if(!name.empty() && (name.GetChar(0) == ':')) { // Which it probably will be
                name = name.AfterFirst(':');
            }
        }
    }

    if(name.empty()) { name << "Folder" << ++counter; }
    wxString newname = wxGetTextFromUser(_("New Virtual Folder Name:"), _("New Virtual Folder"), name);
    newname.Trim().Trim(false);

    if(newname.IsEmpty()) return;

    /*    if ( newname.Contains(":") ) { // No, don't check this: we may have been passed a multi-segment path, or be
       trying to create one
            wxMessageBox(_("':' is not a valid virtual folder character"), "codelite");
            return;
        }*/

    curpath << ":" << newname;
    wxString errmsg;
    if(!clCxxWorkspaceST::Get()->CreateVirtualDirectory(curpath, errmsg, true)) {
        wxMessageBox(_("Error occurred while creating virtual folder:\n") + errmsg, "codelite",
                     wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    m_initialPath = curpath;
    m_reloadTreeNeeded = true;
    DoBuildTree();
}

void VirtualDirectorySelectorDlg::OnNewVDUI(wxUpdateUIEvent& event)
{
    wxTreeItemId id = m_treeCtrl->GetSelection();
    if(id.IsOk() == false) {
        event.Enable(false);
        return;
    }
    MyVdTreeItemData* cd = dynamic_cast<MyVdTreeItemData*>(m_treeCtrl->GetItemData(id));
    event.Enable(cd->IsProject() || cd->IsFolder()); // project or virtual folder
}
