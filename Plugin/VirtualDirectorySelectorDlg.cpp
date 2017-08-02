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
#include <wx/tokenzr.h>
#include "workspace.h"
#include <wx/xrc/xmlres.h>
#include <deque>
#include "bitmap_loader.h"
#include "tree_node.h"
#include <wx/imaglist.h>
#include <wx/regex.h>
#include "plugin.h"
#include "event_notifier.h"
#include "windowattrmanager.h"
#include <wx/wupdlock.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include "globals.h"
#include "imanager.h"
#include "project.h"

VirtualDirectorySelectorDlg::VirtualDirectorySelectorDlg(wxWindow* parent, clCxxWorkspace* wsp,
                                                         const wxString& initialPath, const wxString& projectname)
    : VirtualDirectorySelectorDlgBaseClass(parent)
    , m_workspace(wsp)
    , m_projectName(projectname)
    , m_initialPath(initialPath)
    , m_images(NULL)
    , m_reloadTreeNeeded(false)
{
    m_treeCtrl->SetFocus();
    DoBuildTree();

    GetSizer()->Fit(this);
    CentreOnParent();
    ::MSWSetNativeTheme(m_treeCtrl);
    m_treeCtrlSearchHelper.reset(new clTreeKeyboardInput(m_treeCtrl));
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
}

wxString VirtualDirectorySelectorDlg::DoGetPath(wxTreeCtrl* tree, const wxTreeItemId& item, bool validateFolder)
{
    if(!item.IsOk()) {
        return wxEmptyString;
    }
    if(validateFolder) {
        int imgId = tree->GetItemImage(item);
        if(imgId != 1) { // not a virtual folder
            return wxEmptyString;
        }
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
    wxWindowUpdateLocker locker(m_treeCtrl);
    m_treeCtrl->DeleteAllItems();

    if(m_images == NULL) {
        m_images = new wxImageList(clGetScaledSize(16), clGetScaledSize(16));
        BitmapLoader& bmpLoader = *clGetManager()->GetStdIcons();
        m_images->Add(bmpLoader.LoadBitmap(wxT("cxx-workspace"))); // 0
        m_images->Add(bmpLoader.LoadBitmap(wxT("folder-yellow"))); // 1
        m_images->Add(bmpLoader.LoadBitmap(wxT("project")));       // 2
        m_treeCtrl->AssignImageList(m_images);
    }

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
            if(p) {
                p->GetVirtualDirectories(tree);
            }
        }

        // create the tree
        wxTreeItemId root = m_treeCtrl->AddRoot(nodeData.name, 0, 0);
        tree->GetData().itemId = root;
        TreeWalker<wxString, VisualWorkspaceNode> walker(tree);

        for(; !walker.End(); walker++) {
            // Add the item to the tree
            TreeNode<wxString, VisualWorkspaceNode>* node = walker.GetNode();

            // Skip root node
            if(node->IsRoot()) continue;

            wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
            if(parentHti.IsOk() == false) {
                parentHti = root;
            }

            int imgId(2); // Virtual folder
            switch(node->GetData().type) {
            case ProjectItem::TypeWorkspace:
                imgId = 0;
                break;
            case ProjectItem::TypeProject:
                imgId = 2;
                break;
            case ProjectItem::TypeVirtualDirectory:
            default:
                imgId = 1;
                break;
            }

            // add the item to the tree
            node->GetData().itemId = m_treeCtrl->AppendItem(parentHti,            // parent
                                                            node->GetData().name, // display name
                                                            imgId,                // item image index
                                                            imgId                 // selected item image
                                                            );
            m_treeCtrl->SortChildren(parentHti);
        }

#if !defined(__WXMSW__)
        // For a single project, hide the workspace node. This doesn't work on wxMSW
        if(!m_projectName.empty()) {
            m_treeCtrl->SetWindowStyle(m_treeCtrl->GetWindowStyle() | wxTR_HIDE_ROOT);
            wxTreeItemIdValue cookie;
            wxTreeItemId projitem = m_treeCtrl->GetFirstChild(root, cookie);
            if(projitem.IsOk() && m_treeCtrl->HasChildren(projitem)) {
                m_treeCtrl->Expand(projitem);
            }
        } else
#endif
            if(root.IsOk() && m_treeCtrl->HasChildren(root)) {
            m_treeCtrl->Expand(root);
        }
        delete tree;
    }

    // if a initialPath was provided, try to find and select it
    if(SelectPath(m_initialPath)) {
        m_treeCtrl->Expand(m_treeCtrl->GetSelection());
    }
}

void VirtualDirectorySelectorDlg::OnButtonOkUI(wxUpdateUIEvent& event)
{
    wxTreeItemId id = m_treeCtrl->GetSelection();
    event.Enable(id.IsOk() && m_treeCtrl->GetItemImage(id) == 1);
}

wxTreeItemId VirtualDirectorySelectorDlg::FindItemForPath(const wxString& path)
{
    if(path.empty()) {
        return wxTreeItemId();
    }

    wxArrayString tokens = wxStringTokenize(path, wxT(":"), wxTOKEN_STRTOK);
    wxTreeItemId item = m_treeCtrl->GetRootItem();
    if(m_treeCtrl->GetWindowStyle() & wxTR_HIDE_ROOT) {
        if(!item.IsOk() || !m_treeCtrl->HasChildren(item)) {
            return wxTreeItemId();
        }
    }
    // We need to pump-prime with the first token, otherwise the loop is never entered
    wxTreeItemIdValue cookie;
    item = m_treeCtrl->GetFirstChild(item, cookie);

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
            if(!item.IsOk() || !m_treeCtrl->HasChildren(item)) {
                return false;
            }

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

    if(name.empty()) {
        name << "Folder" << ++counter;
    }
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
    int imgid = m_treeCtrl->GetItemImage(id);
    event.Enable(imgid == 1 || imgid == 2); // project or virtual folder
}
