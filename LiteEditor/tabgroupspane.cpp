//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2010 by Eran Ifrah
// file name            : tabgroupspane.cpp
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

#include "tabgroupspane.h"

#include "clThemedTreeCtrl.h"
#include "codelite_events.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "frame.h"
#include "globals.h"
#include "ieditor.h"
#include "pluginmanager.h"
#include "tabgroupmanager.h"
#include "wx_xml_compatibility.h"

#include <algorithm>
#include <wx/accel.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include <wx/textdlg.h>
#include <wx/wupdlock.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

class MyDropTarget : public wxTextDropTarget
{
    TabgroupsPane* m_pane;

public:
    MyDropTarget(TabgroupsPane* pane)
        : m_pane(pane)
    {
    }
    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data)
    {
        static wxRegEx re("\\{Class:Notebook,TabIndex:([0-9]+)\\}\\{(.*?)\\}", wxRE_ADVANCED);
        if(re.Matches(data)) {
            m_pane->FileDropped(re.GetMatch(data, 2));
        }
        return true;
    }
};

TabgroupsPane::TabgroupsPane(wxWindow* parent, const wxString& caption)
    : wxPanel(parent, wxID_ANY)
{
    m_node = NULL;
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    m_tree = new clThemedTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT);
    m_tree->SetDropTarget(new MyDropTarget(this));

    // Add icons to the tree
    BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
    m_tree->SetBitmaps(bmpLoader->GetStandardMimeBitmapListPtr());

    sz->Add(m_tree, 1, wxEXPAND);

    // DisplayTabgroups(); No, we can't do this here: it's too soon, a 'session' hasn't been loaded
    SetSizer(sz);
    sz->Layout();

    // We can't use an accelerator table here for Cut/Copy/Paste, as the events are hijacked by the frame's shortcuts
    // But delete isn't, so..
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_DELETE);
    wxAcceleratorTable accel(1, entries);
    m_tree->SetAcceleratorTable(accel);

    m_tree->AddRoot(wxT("Tab Groups"), -1, -1, NULL);

    m_tree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(TabgroupsPane::OnItemActivated), NULL, this);
    m_tree->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(TabgroupsPane::OnItemRtClick), NULL, this);
    m_tree->Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(TabgroupsPane::OnBeginLabelEdit), NULL,
                    this);
    m_tree->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(TabgroupsPane::OnEndLabelEdit), NULL, this);
    m_tree->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(TabgroupsPane::OnBeginDrag), NULL, this);
    m_tree->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(TabgroupsPane::OnEndDrag), NULL, this);
    // Context menu:
    Connect(TGM_ID_Add, TGM_ID_RemoveItem, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(TabgroupsPane::OnContextMenu), NULL, this);
    // Shortcuts:
    m_tree->Connect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnCopy), NULL, this);
    m_tree->Connect(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnPaste), NULL, this);
    m_tree->Connect(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnCut), NULL, this);
    m_tree->Connect(wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnDelete), NULL,
                    this);

    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &TabgroupsPane::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &TabgroupsPane::OnInitDone, this);
}

TabgroupsPane::~TabgroupsPane()
{
    m_tree->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(TabgroupsPane::OnItemActivated), NULL,
                       this);
    m_tree->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(TabgroupsPane::OnItemRtClick), NULL, this);
    m_tree->Disconnect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(TabgroupsPane::OnBeginLabelEdit), NULL,
                       this);
    m_tree->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(TabgroupsPane::OnEndLabelEdit), NULL,
                       this);
    m_tree->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(TabgroupsPane::OnBeginDrag), NULL, this);
    m_tree->Disconnect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(TabgroupsPane::OnEndDrag), NULL, this);
    // Context menu:
    Disconnect(TGM_ID_Add, TGM_ID_RemoveItem, wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(TabgroupsPane::OnContextMenu), NULL, this);
    // Shortcuts:
    m_tree->Disconnect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnCopy), NULL,
                       this);
    m_tree->Disconnect(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnPaste), NULL,
                       this);
    m_tree->Disconnect(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnCut), NULL, this);
    m_tree->Disconnect(wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsPane::OnDelete), NULL,
                       this);

    delete m_node;

    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &TabgroupsPane::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &TabgroupsPane::OnInitDone, this);
}

void TabgroupsPane::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    DisplayTabgroups(true); // Display any Global tabgroups
}

bool sortfunction(const spTabGrp& x, const spTabGrp& y) { return x.first.CmpNoCase(y.first) < 0; }

void TabgroupsPane::DisplayTabgroups(bool isGlobal /*=false*/)
{
    vTabGrps& tabgroups = TabGroupsManager::Get()->GetTabgroups(isGlobal);
    std::sort(tabgroups.begin(), tabgroups.end(), sortfunction);
    vTabGrps::const_iterator iter = tabgroups.begin();
    for(; iter != tabgroups.end(); ++iter) {
        AddTreeItem(isGlobal, iter->first, iter->second);
    }
    m_tree->Expand(GetRootItemForTabgroup(isGlobal));
    GetSizer()->Layout();
}

void TabgroupsPane::AddTreeItem(bool isGlobal, const wxString& tabgroupfpath, const wxArrayString& tabfilepaths,
                                const wxTreeItemId insertafter /*=wxTreeItemId()*/)
{
    wxCHECK_RET(!tabgroupfpath.IsEmpty(), wxT("A tabgroup with an empty name in TabgroupsPane::AddTreeItem"));

    // Both for the tabgroup and its constituent files, we display the filename but save the filepaths in the
    // TreeItemData
    wxString tabgroupname = tabgroupfpath.AfterLast(wxFILE_SEP_PATH).BeforeLast('.');
    wxTreeItemId tbnameId;

    int folderImgID = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
    if(insertafter.IsOk()) {
        // There's a valid id to insert after, so..
        tbnameId = m_tree->InsertItem(GetRootItemForTabgroup(isGlobal), insertafter, tabgroupname, folderImgID,
                                      folderImgID, new TabGrpTreeItemData(tabgroupfpath, TGT_group));
    } else {
        // Otherwise, append
        tbnameId = m_tree->AppendItem(GetRootItemForTabgroup(isGlobal), tabgroupname, folderImgID, folderImgID,
                                      new TabGrpTreeItemData(tabgroupfpath, TGT_group));
    }
    wxCHECK_RET(tbnameId.IsOk(), wxT("Failed to add the tabgroup to the tree"));
    // NB. I'm not going to try to cater for idiots who have >1 tab with the same filename!
    for(size_t n = 0; n < tabfilepaths.GetCount(); ++n) {
        wxString name = tabfilepaths.Item(n).AfterLast(wxFILE_SEP_PATH);
        int imgIdx = DoGetIconIndex(name);
        m_tree->AppendItem(tbnameId, name, imgIdx, imgIdx, new TabGrpTreeItemData(tabfilepaths.Item(n)));
    }
}

void TabgroupsPane::OnItemActivated(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if(!item.IsOk()) {
        return;
    }
    TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
    if(!data) {
        return;
    }
    wxString filepath = data->GetFilepath();
    wxFileName fn(filepath);

    if(data->GetType() == TGT_group) { // Activating a tabgroup means offer to load it
        wxArrayString previousgroups;
        EditorConfigST::Get()->GetRecentItems(previousgroups, wxT("RecentTabgroups"));

        if(!fn.FileExists()) {
            wxMessageBox(_("I'm afraid that tabgroup no longer exists :/"), _("CodeLite"), wxICON_ERROR | wxOK, this);
            m_tree->DeleteChildren(item);
            m_tree->Delete(item);
            // Remove it from the RecentTabgroups list too
            int index = previousgroups.Index(filepath);
            if(index != wxNOT_FOUND) {
                previousgroups.RemoveAt(index);
                EditorConfigST::Get()->SetRecentItems(previousgroups, wxT("RecentTabgroups"));
            }
            return;
        }

        std::vector<clEditor*> editors;
        clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached |
                                                                      MainBook::kGetAll_RetainOrder);
        if(editors.size() > 0) {
            // If there are editors currently loaded, ask if they are to be replaced or added to
            wxString msg(_("Do you want to replace the existing editors? (Say 'No' to load the new ones alongside)"));
            int ans = wxMessageBox(msg, _("Load the tabgroup"), wxICON_QUESTION | wxYES_NO | wxCANCEL, this);
            if(ans == wxCANCEL) {
                return;
            }
            if(ans == wxYES) {
                clMainFrame::Get()->GetMainBook()->CloseAll(true);
            }
        }

        clWindowUpdateLocker locker(clMainFrame::Get());
        TabGroupEntry session;
        if(SessionManager::Get().GetSession(filepath.BeforeLast(wxT('.')), session, wxString(wxT("tabgroup")),
                                            tabgroupTag)) {
            clMainFrame::Get()->GetMainBook()->RestoreSession(session);

            // Remove any previous instance of this group from the history, then prepend it and save
            int index = previousgroups.Index(filepath);
            if(index != wxNOT_FOUND) {
                previousgroups.RemoveAt(index);
            }
            previousgroups.Insert(filepath, 0);
            EditorConfigST::Get()->SetRecentItems(previousgroups, wxT("RecentTabgroups"));
        }
    } else {
        // Activating an item means load just it, as an editor
        if(!fn.FileExists()) {
            wxMessageBox(_("I'm afraid that tabgroup item no longer exists :/"), _("CodeLite"), wxICON_ERROR | wxOK,
                         this);
            DeleteTabgroupItem();
            return;
        }

        clMainFrame::Get()->GetMainBook()->OpenFile(filepath);
    }
}

void TabgroupsPane::OnItemRtClick(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if(!item.IsOk()) {
        return;
    }
    TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
    if(!data) {
        return;
    }

    wxMenu menu; // Tabgroup Menu
    if(data->GetType() == TGT_group) {
        menu.Append(TGM_ID_Add, _("&Add a new item to this tabgroup"));
        if(m_node) {
            menu.Append(TGM_ID_Paste, _("&Paste an item into this tabgroup"));
        }
        menu.AppendSeparator();
        menu.Append(TGM_ID_Duplicate, _("Du&plicate this tabgroup"));
        menu.Append(TGM_ID_Delete, _("&Delete this tabgroup"));
    } else {
        menu.Append(TGM_ID_CopyItem, _("&Copy this item, to be pasted to another tabgroup"));
        menu.Append(TGM_ID_CutItem, _("Cu&t this item, to be pasted to another tabgroup"));
        if(m_node) {
            menu.Append(TGM_ID_Paste, _("&Paste an item into this tabgroup"));
        }
        menu.Append(TGM_ID_RemoveItem, _("&Remove this item from the tabgroup"));
    }
    PopupMenu(&menu);
}

void TabgroupsPane::OnContextMenu(wxCommandEvent& event)
{
    switch(event.GetId()) {
    case TGM_ID_Add:
        AddTabgroupItem();
        break;
    case TGM_ID_Paste:
        PasteTabgroupItem();
        break;
    case TGM_ID_Delete:
        DeleteTabgroup();
        break;
    case TGM_ID_Duplicate:
        DuplicateTabgroup();
        break;
    case TGM_ID_CopyItem:
        CopyTabgroupItem();
        break;
    case TGM_ID_CutItem:
        DeleteTabgroupItem(true);
        break;
    case TGM_ID_RemoveItem:
        DeleteTabgroupItem(false);
        break;
    }
}

void TabgroupsPane::OnDelete(wxCommandEvent& WXUNUSED(event)) // From a keypress, not the context menu
{
    wxTreeItemId item = m_tree->GetSelection();
    if(item.IsOk()) {
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if(!data) {
            return;
        }
        if(data->GetType() == TGT_group) {
            DeleteTabgroup();
        } else {
            DeleteTabgroupItem();
        }
    }
}

void TabgroupsPane::OnBeginLabelEdit(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()) {
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if(!data || data->GetType() == TGT_group) {
            // Only allow renaming of a group; renaming an item makes little sense: the name is the filename
            return;
        }
    }

    event.Veto();
}

void TabgroupsPane::OnEndLabelEdit(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if(!item.IsOk()) {
        event.Veto();
        return;
    }

    TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
    if(!data) {
        return;
    }
    wxFileName oldfilepath(data->GetFilepath());

    wxString newfilename = event.GetLabel();
    if(newfilename.IsEmpty() || newfilename == oldfilepath.GetFullName()) {
        event.Veto();
        return;
    }

    wxFileName newfilepath(oldfilepath.GetPath(), newfilename);
    if(newfilepath.FileExists()) {
        wxString msg(_("There is already a file with this name. Overwrite it?"));
        if(wxMessageBox(msg, _("Overwrite?"), wxICON_QUESTION | wxYES_NO, this) != wxYES) {
            event.Veto(); // The user doesn't want to overwrite
            return;
        }
    }
    // If we're here, the event won't be vetoed, so the tree shows the new name
    // Update the file system correspondingly
    if(wxRenameFile(oldfilepath.GetFullPath(), newfilepath.GetFullPath(), true)) {
        clMainFrame::Get()->GetStatusBar()->SetMessage(_("Tabgroup renamed"));
        return;
    }
}

void TabgroupsPane::OnBeginDrag(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()) {
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if(data && data->GetType() == TGT_item) {
            // Only allow dragging of an item, not a group
            m_draggedItem = item;
            event.Allow(); // need explicitly to allow drag
        }
    }
}

void TabgroupsPane::OnEndDrag(wxTreeEvent& event)
{
    if(!m_draggedItem.IsOk()) {
        return;
    }

    wxTreeItemId ItemSrc = m_draggedItem;
    m_draggedItem = wxTreeItemId(); // Reset the dragged-item store

    if(wxGetKeyState(WXK_ESCAPE)) {
        return; // If ESC is being pressed, take this as a hint
    }

    wxTreeItemId ItemDest = event.GetItem();
    if(!ItemDest.IsOk()) {
        return;
    }

    // Are we doing a Move or a Copy/Paste? Use the Ctrl/Alt key to decide
    wxKeyCode copykey =
#ifdef __WXMAC__
        WXK_ALT; // Apparently the ALT key is used for this in Macs
#else
        WXK_CONTROL;
#endif
    if(wxGetKeyState(copykey)) {
        CopyTabgroupItem(ItemSrc);
    } else {
        DeleteTabgroupItem(true, ItemSrc);
    }

    PasteTabgroupItem(ItemDest);
}

void TabgroupsPane::AddTabgroupItem()
{
    wxTreeItemId selection = m_tree->GetSelection();
    if(!selection.IsOk()) {
        return;
    }

    wxString defaultPath;
    IEditor* editor = PluginManager::Get()->GetActiveEditor();
    if(editor) {
        defaultPath = editor->GetFileName().GetPath();
    }

    wxString newfilepath = ::wxFileSelector(_("Select the file you want to add"), defaultPath);
    if(newfilepath.IsEmpty()) {
        return;
    }

    if(!wxFileExists(newfilepath)) {
        if(wxMessageBox(_("That filepath doesn't seem to exist. Are you sure?"), _("CodeLite"),
                        wxICON_QUESTION | wxYES_NO, this) != wxYES) {
            return;
        }
    }

    AddFile(newfilepath);
}

void TabgroupsPane::PasteTabgroupItem(wxTreeItemId itemtopaste /*= wxTreeItemId()*/)
{
    if(m_node == NULL) {
        return; // The data to be pasted should have been stored here, so...
    }

    wxTreeItemId GroupId;
    if(itemtopaste.IsOk()) {
        GroupId = itemtopaste; // We got here from D'n'D, so the item was passed
    } else {
        GroupId = m_tree->GetSelection(); // Just a plain 'Paste'
    }
    if(!GroupId.IsOk()) {
        return;
    }

    // We don't yet know if we're pasting onto a tabgroup or a contained item, so duplicate
    wxTreeItemId ItemId(GroupId);

    TabGrpTreeItemData* groupdata = (TabGrpTreeItemData*)m_tree->GetItemData(GroupId);
    if(!groupdata) {
        return;
    }
    if(groupdata->GetType() == TGT_item) {
        GroupId = m_tree->GetItemParent(ItemId);
        wxCHECK_RET(GroupId.IsOk(), wxT("Trying to paste on something that isn't a tabgroup"));
        groupdata = (TabGrpTreeItemData*)m_tree->GetItemData(GroupId);
    } else {
        // The paste *was* on the group, so null ItemId: we use ItemId.IsOk() later as a flag
        ItemId = wxTreeItemId();
    }
    if(!groupdata) {
        return;
    }
    wxCHECK_RET(groupdata->GetType() == TGT_group, wxT("Trying to paste on something that isn't a tabgroup"));

    wxString filepath = groupdata->GetFilepath();
    wxXmlDocument doc(filepath);
    if(!doc.IsOk()) {
        wxMessageBox(_("Failed to load the destination tabgroup :/"), _("CodeLite"), wxICON_ERROR | wxOK, this);
        return;
    }

    bool NeedToChangeTree = true;
    wxXmlNode* node = TabGroupsManager::Get()->FindTabgroupItem(doc, filepath, m_copieditem_filepath);
    if(node) {
        if(wxMessageBox(_("There is already an item with this filepath in the tabgroup. Overwrite it?"), _("CodeLite"),
                        wxICON_QUESTION | wxYES_NO, this) == wxYES) {
            wxXmlNode* parent = node->GetParent();
            if(parent) {
                parent->RemoveChild(node);
            }
            NeedToChangeTree = false;
        } else {
            return; // The user doesn't want to overwrite
        }
    }

    // If the paste was onto an item (not the group) and there wasn't already an item with this filepath,
    // pass info to DoAddItemToTabgroup() to find the node corresponding to the item, so that we can insert after it in
    // the doc
    wxString nextitemfilepath;
    if(ItemId.IsOk() && NeedToChangeTree) {
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(ItemId);
        if(data) {
            nextitemfilepath = data->GetFilepath();
        }
    }

    if(TabGroupsManager::Get()->DoAddItemToTabgroup(doc, new wxXmlNode(*m_node), filepath, nextitemfilepath)) {
        doc.Save(filepath);

        if(NeedToChangeTree) {
            // We're not overwriting a pre-existing tab, so add the pasted tab to the tree
            wxString name = m_copieditem_filepath.AfterLast(wxFILE_SEP_PATH);
            int imgIdx = DoGetIconIndex(name);
            if(ItemId.IsOk()) {
                // either after the selected item (if there was one)..
                m_tree->InsertItem(GroupId, ItemId, name, imgIdx, imgIdx,
                                   new TabGrpTreeItemData(m_copieditem_filepath));
            } else {
                // ..or append to the group
                m_tree->AppendItem(GroupId, name, imgIdx, imgIdx, new TabGrpTreeItemData(m_copieditem_filepath));
            }
        }

        clMainFrame::Get()->GetStatusBar()->SetMessage(_("Tabgroup item pasted"));
    }
}

void TabgroupsPane::DeleteTabgroup()
{
    wxTreeItemId item = m_tree->GetSelection();
    if(!item.IsOk()) {
        return;
    }
    TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
    if(!data) {
        return;
    }
    wxCHECK_RET(data->GetType() == TGT_group, wxT("The selection wasn't a tabgroup"));

    wxString filepath = data->GetFilepath();
    if(!wxFileName::FileExists(filepath)) {
        m_tree->DeleteChildren(item);
        m_tree->Delete(item);
        return;
    }

    wxString msg = wxString::Format(_("Delete tabgroup %s?"), m_tree->GetItemText(item).c_str());
    if(wxMessageBox(msg, _("CodeLite"), wxICON_QUESTION | wxYES_NO, this) == wxYES) {
        m_tree->Delete(item);

        // Remove it from the RecentTabgroups list too
        wxArrayString previousgroups;
        EditorConfigST::Get()->GetRecentItems(previousgroups, wxT("RecentTabgroups"));
        int index = previousgroups.Index(filepath);
        if(index != wxNOT_FOUND) {
            previousgroups.RemoveAt(index);
            EditorConfigST::Get()->SetRecentItems(previousgroups, wxT("RecentTabgroups"));
        }

        clRemoveFile(filepath);

        clMainFrame::Get()->GetStatusBar()->SetMessage(_("Tabgroup deleted"));
    }
}

void TabgroupsPane::DuplicateTabgroup()
{
    wxTreeItemId selection = m_tree->GetSelection();
    if(!selection.IsOk()) {
        return;
    }
    TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(selection);
    if(!data) {
        return;
    }

    wxCHECK_RET(data->GetType() == TGT_group, wxT("The selection wasn't a tabgroup"));

    wxFileName oldfilepath(data->GetFilepath());
    wxCHECK_RET(oldfilepath.FileExists(), wxT("There was no file corresponding to the tree selection"));

    wxString newname = wxGetTextFromUser(_("What name to you want to give the duplicated tabgroup?"),
                                         _("Duplicate a tabgroup"), oldfilepath.GetFullName(), this);
    if(newname.IsEmpty() || newname == oldfilepath.GetFullName()) {
        return;
    }

    if(!newname.EndsWith(".tabgroup")) {
        newname << ".tabgroup"; // Otherwise things break
    }
    wxFileName newfilepath(oldfilepath.GetPath(), newname);
    if(newfilepath.FileExists()) {
        wxMessageBox(_("Sorry, there is already a tabgroup with this name"), _("CodeLite"), wxICON_INFORMATION | wxOK,
                     this);
        return;
    }

    if(!wxCopyFile(oldfilepath.GetFullPath(), newfilepath.GetFullPath())) {
        wxMessageBox(_("Sorry, duplication of the tabgroup failed :/"), _("CodeLite"), wxICON_ERROR | wxOK, this);
        return;
    }
    wxTreeItemId parent = m_tree->GetItemParent(selection);
    wxCHECK_RET(parent.IsOk(), "A parentless tabgroup");
    TabGrpTreeItemData* parentdata = (TabGrpTreeItemData*)m_tree->GetItemData(parent);
    wxCHECK_RET(parentdata, "An unnamed tabgroup parent");
    bool isGlobal = parentdata->GetFilepath().StartsWith(_("Global"));

    // Do the rest in a separate method, which is also called by Frame::OnFileSaveTabGroup
    if(AddNewTabgroupToTree(isGlobal, newfilepath.GetFullPath(), selection)) {
        clMainFrame::Get()->GetStatusBar()->SetMessage(_("Tabgroup duplicated"));
    }
}

bool TabgroupsPane::AddNewTabgroupToTree(bool isGlobal, const wxString& newfilepath,
                                         wxTreeItemId selection /*=wxTreeItemId()*/)
{
    // Tell TabGroupsManager to load the new group. Probably a good idea anyway, but we'll need it below
    TabGroupsManager::Get()->LoadTabgroupData(isGlobal, newfilepath);
    // Now add the newly-loaded info to the tree
    wxArrayString items;
    if(TabGroupsManager::Get()->FindTabgroup(isGlobal, newfilepath, items)) {
        AddTreeItem(isGlobal, newfilepath, items, selection);
        return true;
    }

    return false;
}

void TabgroupsPane::CopyTabgroupItem(wxTreeItemId itemtocopy /*= wxTreeItemId()*/)
{
    wxTreeItemId item;
    if(itemtocopy.IsOk()) {
        item = itemtocopy; // We got here from D'n'D, so the item was passed
    } else {
        item = m_tree->GetSelection(); // Just a plain 'Copy'
    }
    if(!item.IsOk()) {
        return;
    }

    TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
    if(!data) {
        return;
    }
    if(data->GetType() != TGT_item) {
        return; // I can't think of any reason to copy a whole group
    }

    wxTreeItemId GroupId = m_tree->GetItemParent(item);
    TabGrpTreeItemData* groupdata = (TabGrpTreeItemData*)m_tree->GetItemData(GroupId);
    if(!groupdata) {
        return;
    }

    wxString filepath = groupdata->GetFilepath();
    wxXmlDocument doc(filepath);
    wxCHECK_RET(doc.IsOk(), wxT("Failed to load the tabgroup to be copied :/"));

    wxString itemfilepath = data->GetFilepath();
    wxXmlNode* node = TabGroupsManager::Get()->FindTabgroupItem(doc, filepath, itemfilepath);
    wxCHECK_RET(node != NULL, wxT("Failed to find the tab-item to be copied :/"));

    // All is well, so store the data in m_node and m_copieditem_filepath
    delete m_node;
    m_node = new wxXmlNode(*node);
    m_copieditem_filepath = itemfilepath;
    clMainFrame::Get()->GetStatusBar()->SetMessage(_("Tabgroup item copied"));
}

void TabgroupsPane::DeleteTabgroupItem(bool DoCut /*=false*/, wxTreeItemId itemtocut /*= wxTreeItemId()*/)
{
    wxTreeItemId item;
    if(itemtocut.IsOk()) {
        item = itemtocut; // We got here from D'n'D, so the item was passed
    } else {
        item = m_tree->GetSelection(); // Just a plain 'Cut' or 'Delete'
    }
    if(!item.IsOk()) {
        return;
    }
    TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
    if(!data) {
        return;
    }
    wxCHECK_RET(data->GetType() == TGT_item, wxT("The selection was a tabgroup, not a tabgroup item"));
    wxString itemfilepath = data->GetFilepath(); // Need to do this here, before the item is deleted!
    wxTreeItemId parent = m_tree->GetItemParent(item);
    TabGrpTreeItemData* tabgrpdata = (TabGrpTreeItemData*)m_tree->GetItemData(parent);
    if(!tabgrpdata) {
        return;
    }
    wxString filepath = tabgrpdata->GetFilepath();
    if(!wxFileName::FileExists(filepath)) {
        return;
    }

    if(!DoCut) { // If we're deleting, check the user really meant it
        wxString msg = wxString::Format(_("Remove item %s from %s?"), m_tree->GetItemText(item).c_str(),
                                        m_tree->GetItemText(parent).c_str());
        if(wxMessageBox(msg, _("CodeLite"), wxICON_QUESTION | wxYES_NO, this) != wxYES) {
            return;
        }
    }
    m_tree->Delete(item);

    wxXmlDocument doc(filepath);
    if(doc.IsOk()) {
        wxXmlNode* TabInfoNode = TabGroupsManager::Get()->DoDeleteTabgroupItem(doc, filepath, itemfilepath);
        if(TabInfoNode) {
            if(DoCut) {
                // If we're cutting, store the deleted node ready for paste
                m_node = new wxXmlNode(*TabInfoNode);
                m_copieditem_filepath = itemfilepath;
                clMainFrame::Get()->GetStatusBar()->SetMessage(_("Tabgroup item Cut"));
            } else {
                clMainFrame::Get()->GetStatusBar()->SetMessage(_("Tabgroup item deleted"));
            }
            return;
        }
    }

    // Either !doc.IsOk() or !TabInfoNode
    wxMessageBox(_("Failed to amend the tabgroup :/"), _("CodeLite"), wxICON_ERROR | wxOK, this);
}

int TabgroupsPane::DoGetIconIndex(const wxString& filename)
{
    int imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(filename);
    if(imgIdx == wxNOT_FOUND) {
        imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeText);
    }
    return imgIdx;
}

void TabgroupsPane::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    m_tree->DeleteChildren(GetRootItemForTabgroup(false));
}

void TabgroupsPane::FileDropped(const wxString& filename)
{
    // An editor was DnD to the TabGroup pane view
    AddFile(filename);
}

void TabgroupsPane::AddFile(const wxString& filename)
{
    // Create a suitable node, then piggyback on PasteTabgroupItem() to do the real work
    // Reuse m_copieditem_filepath and m_node for this, so cache them first
    wxString oldcopieditem_filepath = m_copieditem_filepath;
    wxXmlNode* oldnode = m_node;

    m_copieditem_filepath = filename;
    m_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("TabInfo"));
    wxXmlNode* fp = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("wxString"));
    fp->AddAttribute(wxT("Value"), filename);
    fp->AddAttribute(wxT("Name"), wxT("FileName"));
    wxXmlNode* fvl = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("int"));
    fvl->AddAttribute(wxT("Name"), wxT("FirstVisibleLine"));
    wxXmlNode* cl = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("int"));
    cl->AddAttribute(wxT("Name"), wxT("CurrentLine"));
    wxXmlNode* bm = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("wxArrayString"));
    bm->AddAttribute(wxT("Name"), wxT("Bookmarks"));

    PasteTabgroupItem();

    // Restore the old values
    m_copieditem_filepath = oldcopieditem_filepath;
    wxDELETE(m_node);
    m_node = oldnode;
}

wxTreeItemId TabgroupsPane::GetRootItemForTabgroup(bool global)
{
    wxString label = global ? _("Global tabgroups") : _("Workspace tabgroups");

    // First see if the appropriate tabgroup base-item has already been created
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_tree->GetFirstChild(m_tree->GetRootItem(), cookie);
    while(child.IsOk()) {
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(child);
        if(data && data->GetFilepath() == label) {
            return child;
        }
        child = m_tree->GetNextSibling(child);
    }
    // Nope, so create it
    int folderImgID = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
    return m_tree->AppendItem(m_tree->GetRootItem(), label, folderImgID, folderImgID,
                              new TabGrpTreeItemData(label, TGT_group));
}
