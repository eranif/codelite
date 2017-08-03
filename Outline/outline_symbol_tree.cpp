//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cpp_symbol_tree.cpp
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
#include "precompiled_header.h"
#include <wx/thread.h>
#include "globals.h"
#include "crawler_include.h"
#include "stringsearcher.h"
#include "stringsearcher.h"
#include <wx/stc/stc.h>
#include "fc_fileopener.h"
#include "macros.h"

#include "outline_symbol_tree.h"
#include <algorithm>
#include "imanager.h"

//#include "manager.h"
//#include "frame.h"
#include "bitmap_loader.h"
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>

#define INCLUDE_FILES_NODE_TEXT _("Include Files")

IMPLEMENT_DYNAMIC_CLASS(svSymbolTree, SymbolTree)

const wxEventType wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED = wxNewEventType();

//----------------------------------------------------------------
// accessory function
//----------------------------------------------------------------
wxImageList* svSymbolTree::CreateSymbolTreeImages()
{
    wxImageList* images = new wxImageList(clGetScaledSize(16), clGetScaledSize(16), true);

    BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
    images->Add(bmpLoader->LoadBitmap(wxT("mime-cpp")));                 // 0
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/namespace")));          // 1
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/globals")));            // 2
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/class")));              // 3
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/struct")));             // 4
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_public")));    // 5
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_protected"))); // 6
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_private")));   // 7
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_public")));      // 8
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_protected")));   // 9
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_private")));     // 10
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/typedef")));            // 11
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/macro")));              // 12
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/enum")));               // 13
    images->Add(bmpLoader->LoadBitmap(wxT("cc/16/enumerator")));         // 14
    images->Add(bmpLoader->LoadBitmap(wxT("mime-cpp")));                 // 15
    images->Add(bmpLoader->LoadBitmap(wxT("mime-h")));                   // 16
    return images;
}

svSymbolTree::svSymbolTree()
    : m_uid(0)
{
    m_sortByLineNumber = true;
}

svSymbolTree::svSymbolTree(
    wxWindow* parent, IManager* manager, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : SymbolTree(parent, id, pos, size, style)
    , m_uid(0)
{
    m_sortByLineNumber = true;
    m_manager = manager;
    Connect(GetId(), wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(svSymbolTree::OnMouseRightUp));
    Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(svSymbolTree::OnMouseDblClick));
    Connect(GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(svSymbolTree::OnItemActivated));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(svSymbolTree::OnMouseDblClick), NULL, this);
    Connect(wxEVT_PARSE_INCLUDE_STATEMENTS_DONE, wxCommandEventHandler(svSymbolTree::OnIncludeStatements), NULL, this);
    MSWSetNativeTheme(this);
    // SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
}

void svSymbolTree::OnMouseRightUp(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()) {
        SelectItem(item, true);
        DoItemActivated(item, event, true);
    }
}

void svSymbolTree::OnMouseDblClick(wxMouseEvent& event)
{
    // Make sure the double click was done on an actual item
    int flags = 0;
    wxTreeItemId where = HitTest(event.GetPosition(), flags);

    if(where.IsOk() && (flags & wxTREE_HITTEST_ONITEMLABEL)) {
        SelectItem(where);
        DoItemActivated(where, event, true);

    } else {
        event.Skip();
    }
}

bool svSymbolTree::ActivateSelectedItem()
{
    wxTreeItemId item = GetSelection();
    wxTreeEvent dummy;
    return DoItemActivated(item, dummy, true);
}

bool svSymbolTree::DoItemActivated(wxTreeItemId item, wxEvent& event, bool notify)
{
    //-----------------------------------------------------
    // Each tree items, keeps a private user data that
    // holds the key for searching the its corresponding
    // node in the m_tree data structure
    //-----------------------------------------------------
    if(item.IsOk() == false) return false;

    MyTreeItemData* itemData = static_cast<MyTreeItemData*>(GetItemData(item));
    if(!itemData) {
        event.Skip();
        return false;
    }

    wxString filename = itemData->GetFileName();
    wxString project = m_manager->GetWorkspace()->GetActiveProjectName();
    wxString pattern = itemData->GetPattern();
    // int      lineno  = itemData->GetLine();

    /*
        // Open the file and set the cursor to line number
        if(clMainFrame::Get()->GetMainBook()->OpenFile(filename, project, lineno-1)) {
                // get the editor, and search for the pattern in the file
                LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
                if (editor) {
                        FindAndSelect(editor, pattern, GetItemText(item));
                }
        }
    */
    FindAndSelect(m_manager->GetActiveEditor(), pattern, GetItemText(item));

    // post an event that an item was activated
    if(notify) {
        wxCommandEvent e(wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED);
        e.SetEventObject(this);
        wxPostEvent(GetEventHandler(), e);
    }
    return true;
}

void svSymbolTree::OnItemActivated(wxTreeEvent& event)
{
    if(event.GetKeyCode() == WXK_RETURN) {
        wxTreeItemId item = GetSelection();
        DoItemActivated(item, event, true);
    } else {
        event.Skip();
    }
}

void svSymbolTree::AdvanceSelection(bool forward)
{
    wxTreeItemId item = GetSelection();
    if(!item.IsOk()) {
        return;
    }

    wxTreeItemId nextItem;
    if(forward) {
        // Item is visible, scroll to it to make sure GetNextVisible() wont
        // fail
        ScrollTo(item);
        nextItem = GetNextVisible(item);
    } else {
        nextItem = TryGetPrevItem(item);
    }

    if(nextItem.IsOk()) {
        SelectItem(nextItem);
    }
}

wxTreeItemId svSymbolTree::TryGetPrevItem(wxTreeItemId item)
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), wxT("invalid tree item"));

    // find out the starting point
    wxTreeItemId prevItem = GetPrevSibling(item);
    if(!prevItem.IsOk()) {
        prevItem = GetItemParent(item);
    }

    // from there we must be able to navigate until this item
    while(prevItem.IsOk()) {
        ScrollTo(prevItem);
        const wxTreeItemId nextItem = GetNextVisible(prevItem);
        if(!nextItem.IsOk() || nextItem == item) return prevItem;

        prevItem = nextItem;
    }

    return wxTreeItemId();
}

void svSymbolTree::FindAndSelect(IEditor* editor, wxString& pattern, const wxString& name)
{
    // sanity
    CHECK_PTR_RET(editor);

    if(editor->FindAndSelect(pattern, name, 0 /* from pos */, m_manager->GetNavigationMgr()) == false) {
        // Could not select, clear the selection
        editor->GetCtrl()->SetSelectionStart(wxNOT_FOUND);
        editor->GetCtrl()->SetSelectionEnd(wxNOT_FOUND);
    }

    m_manager->GetActiveEditor()->GetCtrl()->SetSTCFocus(true);
    m_manager->GetActiveEditor()->GetCtrl()->SetFocus();

    // If the editor has folds and the target line is above the current position,
    // the stc doesn't scroll back far enough to show the target. So:
    CallAfter(&svSymbolTree::CenterEditorLine);
}

void svSymbolTree::CenterEditorLine()
{
    IEditor* editor = m_manager->GetActiveEditor();
    if (editor) {
        int lineno = editor->GetCurrentLine();
        editor->CenterLine(lineno);
    }
}

void svSymbolTree::BuildTree(const wxFileName& fn, bool forceBuild)
{
    TagEntryPtrVector_t newTags;
    ITagsStoragePtr db = TagsManagerST::Get()->GetDatabase();
    if(!db) {
        return;
    }

    db->SelectTagsByFile(fn.GetFullPath(), newTags);
    if(!forceBuild && TagsManagerST::Get()->AreTheSame(newTags, m_currentTags)) return;

    if(m_sortByLineNumber) {
        std::sort(newTags.begin(), newTags.end(),
            [&](TagEntryPtr t1, TagEntryPtr t2) { return t1->GetLine() > t2->GetLine(); });
    }
    wxWindowUpdateLocker locker(this);
    SymbolTree::BuildTree(fn, newTags, forceBuild);

    // Request from the parsing thread list of include files
    ++m_uid;

    ParseRequest* req = new ParseRequest(this);
    req->setFile(fn.GetFullPath());
    req->setType(ParseRequest::PR_PARSE_INCLUDE_STATEMENTS);
    req->_uid = m_uid; // Identifies this request
    ParseThreadST::Get()->Add(req);

    wxTreeItemId root = GetRootItem();
    if(root.IsOk() && ItemHasChildren(root)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(root, cookie);
        while(child.IsOk()) {
            Expand(child);
            child = GetNextChild(root, cookie);
        }
    }
}

wxTreeItemId svSymbolTree::DoAddIncludeFiles(const wxFileName& fn, const fcFileOpener::List_t& includes)
{
    wxTreeItemId root = GetRootItem();

    if(root.IsOk() == false) return wxTreeItemId();

    if(root.IsOk() && ItemHasChildren(root)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(root, cookie);
        while(child.IsOk()) {

            // Dont add duplicate items
            if(GetItemText(child) == INCLUDE_FILES_NODE_TEXT) {
                Delete(child);
                break;
            }
            child = GetNextChild(root, cookie);
        }
    }

    if(includes.empty()) return wxTreeItemId();

    wxTreeItemId item;
    if(ItemHasChildren(root)) {
        item = InsertItem(
            root, 0, INCLUDE_FILES_NODE_TEXT, 2, 2, new MyTreeItemData(INCLUDE_FILES_NODE_TEXT, wxEmptyString));

    } else {
        item =
            AppendItem(root, INCLUDE_FILES_NODE_TEXT, 2, 2, new MyTreeItemData(INCLUDE_FILES_NODE_TEXT, wxEmptyString));
    }

    fcFileOpener::List_t::const_iterator iter = includes.begin();
    for(; iter != includes.end(); ++iter) {
        wxString displayName(*iter);
        AppendItem(item, displayName, 16, 16, new MyTreeItemData(displayName, displayName));
    }
    return item;
}

bool svSymbolTree::IsSelectedItemIncludeFile()
{
    wxTreeItemId item = GetSelection();
    if(item.IsOk() == false || item == GetRootItem()) return false;

    wxTreeItemId parent = GetItemParent(item);
    if(parent.IsOk() == false || parent == GetRootItem()) return false;

    return GetItemText(parent) == INCLUDE_FILES_NODE_TEXT;
}

wxString svSymbolTree::GetSelectedIncludeFile() const
{
    wxString included_file;
    wxTreeItemId item = GetSelection();
    if(item.IsOk() == false || item == GetRootItem()) return wxT("");

    included_file = GetItemText(item);
    included_file.Replace(wxT("\""), wxT(""));
    included_file.Replace(wxT(">"), wxT(""));
    included_file.Replace(wxT("<"), wxT(""));
    return included_file;
}

void svSymbolTree::Clear()
{
    SymbolTree::Clear();
    m_uid++;
}

void svSymbolTree::OnIncludeStatements(wxCommandEvent& e)
{
    fcFileOpener::List_t* includes = (fcFileOpener::List_t*)e.GetClientData();
    if(includes) {
        if(m_uid == e.GetInt()) {
            wxWindowUpdateLocker locker(this);
            wxTreeItemId item = DoAddIncludeFiles(m_fileName, *includes);
            wxUnusedVar(item);
            // if(item.IsOk()) {
            //     //                if(ItemHasChildren(item)) {
            //     //                    Expand(item);
            //     //                }
            //     ScrollTo(item);
            // }
        }
        includes->clear();
        delete includes;
    }
}

void svSymbolTree::ClearCache() { m_currentTags.clear(); }
