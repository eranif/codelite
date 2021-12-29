//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : new_quick_watch_dlg.cpp
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

#include "new_quick_watch_dlg.h"

#include "clDebuggerEditItemDlg.h"
#include "debuggerobserver.h"
#include "editor_config.h"
#include "frame.h"
#include "globals.h"
#include "simpletable.h"
#include "windowattrmanager.h"
#include "wx/persist/window.h"
#include "wx/popupwin.h"

#include <cmath>
#include <wx/cursor.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/timer.h>
#include <wx/xrc/xmlres.h>

static wxRect s_Rect;

class QWTreeData : public wxTreeItemData
{
public:
    VariableObjChild _voc;

    QWTreeData(const VariableObjChild& voc)
        : _voc(voc)
    {
    }
    virtual ~QWTreeData() {}
};

#if wxVERSION_NUMBER >= 3104 && defined(__WXGTK3__)
static void DoNothing(wxShowEvent& event)
{
    // Swallows wxEVT_SHOW, which would otherwise result in a zero clientsize due
    // to a side-effect of the 3rd fix in https://trac.wxwidgets.org/ticket/16088
}
#endif

DisplayVariableDlg::DisplayVariableDlg(wxWindow* parent)
    : clResizableTooltip(parent)
    , m_debugger(NULL)
    , m_editDlgIsUp(false)
{
    Hide();
    Centre();
    SetName("clDebuggerEditItemDlgBase");
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_MENU, &DisplayVariableDlg::OnItemMenu, this);
#if wxVERSION_NUMBER >= 3104 && defined(__WXGTK3__)
    Bind(wxEVT_SHOW, DoNothing);
#endif
}

DisplayVariableDlg::~DisplayVariableDlg()
{
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_MENU, &DisplayVariableDlg::OnItemMenu, this);
}

void DisplayVariableDlg::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()) {
        if(m_treeCtrl->ItemHasChildren(item)) {
            wxTreeItemIdValue kookie;
            wxTreeItemId child = m_treeCtrl->GetFirstChild(item, kookie);
            while(child.IsOk()) {
                if(m_treeCtrl->GetItemText(child) == wxT("<dummy>")) {
                    // Dummy node, remove it and ask the debugger for information
                    m_treeCtrl->SetItemText(child, _("Loading..."));

                    QWTreeData* data = (QWTreeData*)m_treeCtrl->GetItemData(item);
                    if(data) {
                        // Ask the debugger for information
                        m_debugger->ListChildren(data->_voc.gdbId, DBG_USERR_QUICKWACTH);
                        m_gdbId2Item[data->_voc.gdbId] = item;
                    }
                    break;
                }
                child = m_treeCtrl->GetNextChild(item, kookie);
            }
        }
    }
}

void DisplayVariableDlg::BuildTree(const VariableObjChildren& children, IDebugger* debugger)
{
    m_debugger = debugger;
    m_gdbId2Item.clear();
    m_gdbId2ItemLeaf.clear();
    m_treeCtrl->DeleteAllItems();

    VariableObjChild vob;
    vob.gdbId = m_mainVariableObject;
    vob.isAFake = false;

    wxTreeItemId root = m_treeCtrl->AddRoot(m_variableName, -1, -1, new QWTreeData(vob));

    // Mac does not return value for the root item...
    // we need to force another evaluate call here
#ifdef __WXMAC__
    m_debugger->EvaluateVariableObject(m_mainVariableObject, DBG_USERR_QUICKWACTH);
    m_gdbId2ItemLeaf[m_mainVariableObject] = root;
#endif

    if(children.empty())
        return;
    DoAddChildren(root, children);
}

void DisplayVariableDlg::AddItems(const wxString& varname, const VariableObjChildren& children)
{
    auto iter = m_gdbId2Item.find(varname);
    if(iter != m_gdbId2Item.end()) {
        wxTreeItemId item = iter->second;
        DoAddChildren(item, children);
    }
}

void DisplayVariableDlg::DoAddChildren(wxTreeItemId& item, const VariableObjChildren& children)
{
    if(item.IsOk() == false)
        return;

    if(m_treeCtrl->GetRootItem() != item && m_treeCtrl->ItemHasChildren(item)) {
        // delete the <dummy> node
        wxTreeItemIdValue kookie;
        wxTreeItemId child = m_treeCtrl->GetFirstChild(item, kookie);
        while(child.IsOk()) {
            wxString itemText = m_treeCtrl->GetItemText(child);
            if(itemText == wxT("<dummy>") || itemText == _("Loading...")) {
                m_treeCtrl->Delete(child);
            }
            child = m_treeCtrl->GetNextChild(item, kookie);
        }
    }

    for(size_t i = 0; i < children.size(); i++) {
        const VariableObjChild& ch = children[i];

        // Dont use ch.isAFake here since it will also returns true of inheritance
        if(ch.varName != "public" && ch.varName != "private" && ch.varName != "protected") {
            // Real node
            wxTreeItemId child = m_treeCtrl->AppendItem(item, ch.varName, -1, -1, new QWTreeData(ch));
            if(ch.numChilds > 0) {
                // add fake node to this item, so it will have the [+] on the side
                m_treeCtrl->AppendItem(child, wxT("<dummy>"));
            }

            // ask gdb for the value for this node

            m_debugger->EvaluateVariableObject(ch.gdbId, DBG_USERR_QUICKWACTH);
            m_gdbId2ItemLeaf[ch.gdbId] = child;

        } else {

            // Fake node, ask for expansion only if this node is visible
            m_debugger->ListChildren(ch.gdbId, DBG_USERR_QUICKWACTH);
            m_gdbId2Item[ch.gdbId] = item;
        }
    }
}

void DisplayVariableDlg::OnBtnCancel(wxCommandEvent& e)
{
    DoCleanUp();
    e.Skip();
}

void DisplayVariableDlg::UpdateValue(const wxString& varname, const wxString& value)
{
    wxTreeItemId nodeId;
    auto iter = m_gdbId2ItemLeaf.find(varname);
    if(iter != m_gdbId2ItemLeaf.end()) {
        wxTreeItemId item = iter->second;
        if(item.IsOk()) {
            wxString curtext = m_treeCtrl->GetItemText(item);
#ifdef __WXMAC__
            if(item == m_treeCtrl->GetRootItem()) {
                curtext = curtext.BeforeFirst(wxT('='));
            }
#endif
            curtext << wxT(" = ") << value;
            m_treeCtrl->SetItemText(item, curtext);

        } else if(item.IsOk()) {
            nodeId = item;
        }
    } else if(varname == m_mainVariableObject) {

        // Handle Root
        nodeId = m_treeCtrl->GetRootItem();
    }
}

void DisplayVariableDlg::OnCloseEvent(wxCloseEvent& e)
{
    DoCleanUp();
    e.Skip();
}

void DisplayVariableDlg::DoCleanUp()
{
    if(m_debugger && m_mainVariableObject.IsEmpty() == false) {
        m_debugger->DeleteVariableObject(m_mainVariableObject);
    }
    m_gdbId2Item.clear();
    m_gdbId2ItemLeaf.clear();
    m_mainVariableObject = wxT("");
    m_variableName = wxT("");
    m_expression = wxT("");
    m_itemOldValue.Clear();
    m_editDlgIsUp = false;
    wxSetCursor(wxNullCursor);
}

void DisplayVariableDlg::HideDialog()
{
    DoCleanUp();
    // asm("int3");
    clResizableTooltip::Hide();
}

void DisplayVariableDlg::ShowDialog(bool center)
{
    wxUnusedVar(center);
    clResizableTooltip::ShowTip();
}

void DisplayVariableDlg::OnItemMenu(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();

    if(item.IsOk()) {
        m_treeCtrl->SelectItem(item);
    }

    // Dont show popup menu for fake nodes
    if(IsFakeItem(item))
        return;

    // Popup the menu
    wxMenu menu;

    menu.Append(XRCID("tip_add_watch"), _("Add Watch"));
    menu.Append(XRCID("tip_copy_value"), _("Copy Value to Clipboard"));
    menu.Append(XRCID("edit_item"), _("Edit..."));

    menu.Connect(XRCID("tip_add_watch"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DisplayVariableDlg::OnMenuSelection), NULL, this);
    menu.Connect(XRCID("tip_copy_value"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DisplayVariableDlg::OnMenuSelection), NULL, this);
    menu.Connect(XRCID("edit_item"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DisplayVariableDlg::OnMenuSelection), NULL, this);

    m_treeCtrl->PopupMenu(&menu);
}

wxString DisplayVariableDlg::DoGetItemPath(const wxTreeItemId& treeItem)
{
    wxString fullpath;
    wxTreeItemId item = treeItem;
    while(item.IsOk()) {
        wxString text = m_treeCtrl->GetItemText(item);
        text = text.BeforeFirst(wxT('='));

#ifdef __WXMAC__
        // Mac puts the type in square brackets, remove them as well
        text = text.BeforeFirst(wxT('['));
#endif
        text.Trim().Trim(false);

        if(item != m_treeCtrl->GetRootItem()) {
            if(IsFakeItem(item) == false) {
                text.Prepend(wxT("."));
                fullpath.Prepend(text);
            }
        } else {
            // Root item
            fullpath.Prepend(text);
        }

        // Are we at root yet?
        if(m_treeCtrl->GetRootItem() == item)
            break;

        // Surround this expression with parenthesiss
        item = m_treeCtrl->GetItemParent(item);
    }

    wxString exprWithParentheses;
    wxArrayString items = ::wxStringTokenize(fullpath, wxT("."), wxTOKEN_STRTOK);
    for(size_t i = 0; i < items.GetCount(); i++) {
        exprWithParentheses << items.Item(i);
        exprWithParentheses.Prepend(wxT("(")).Append(wxT(")."));
    }

    if(!items.IsEmpty()) {
        exprWithParentheses.RemoveLast();
    }

    return exprWithParentheses;
}

bool DisplayVariableDlg::IsFakeItem(const wxTreeItemId& item)
{
    if(item.IsOk() == false)
        return true; // fake

    if(item != m_treeCtrl->GetRootItem()) {
        QWTreeData* data = (QWTreeData*)m_treeCtrl->GetItemData(item);
        if(data)
            return data->_voc.isAFake;

        return false;

    } else {
        return false;
    }
}

void DisplayVariableDlg::OnMenuSelection(wxCommandEvent& e)
{
    wxTreeItemId item = m_treeCtrl->GetSelection();
    if(item.IsOk() && !IsFakeItem(item)) {
        if(e.GetId() == XRCID("tip_add_watch")) {
            wxString fullpath = DoGetItemPath(item);
            clMainFrame::Get()->GetDebuggerPane()->GetWatchesTable()->AddExpression(fullpath);
            clMainFrame::Get()->GetDebuggerPane()->SelectTab(wxGetTranslation(DebuggerPane::WATCHES));
            clMainFrame::Get()->GetDebuggerPane()->GetWatchesTable()->RefreshValues();

        } else if(e.GetId() == XRCID("tip_copy_value")) {
            wxString itemText = m_treeCtrl->GetItemText(item);
            itemText = itemText.AfterFirst(wxT('='));
            CopyToClipboard(itemText.Trim().Trim(true));

        } else if(e.GetId() == XRCID("edit_item")) {
            DoEditItem(item);
        }
    }
}

void DisplayVariableDlg::OnCreateVariableObjError(const DebuggerEventData& event) { wxUnusedVar(event); }

void DisplayVariableDlg::DoEditItem(const wxTreeItemId& item)
{
    if(item.IsOk() == false)
        return;

    wxString oldText = m_treeCtrl->GetItemText(item);
    oldText = oldText.BeforeFirst(wxT('='));
    oldText.Trim().Trim(false);

#ifdef __WXGTK__
    wxPoint oldPos = ::wxGetMousePosition();
    oldPos = ScreenToClient(oldPos);
#endif

    m_editDlgIsUp = true;
    clDebuggerEditItemDlg dlg(this, oldText);
    // We need to Hide() the tip before running the edit dialog, otherwise the dialog is covered by the tip
    // (and can't be entered or cancelled...
    Hide();
    int res = dlg.ShowModal();
    Show();
    m_editDlgIsUp = false;

#ifdef __WXGTK__
    wxWindow::WarpPointer(oldPos.x, oldPos.y);
#endif

    if(res != wxID_OK) {
        return;
    }

    wxString newText = dlg.GetValue();
    if(newText.IsEmpty())
        return;

    wxString newExpr = DoGetItemPath(item);
    m_treeCtrl->SetItemText(item, newText);

    // Create a new expression and ask GDB to evaluate it for us
    wxString typecast = newText;
    typecast.Trim().Trim(false);

    int where = typecast.Find(oldText);
    if(where == wxNOT_FOUND || where == 0) {
        // The new text edited by the user does not contain the "old" expression
        // OR it does contain it, but with an additional text to the END
        newExpr = DoGetItemPath(item);

    } else {
        typecast.Replace(oldText, wxT(""));
        typecast.Trim().Trim(false);

        if(!typecast.IsEmpty()) {
            if(!typecast.StartsWith(wxT("(")))
                typecast.Prepend(wxT("("));

            if(!typecast.EndsWith(wxT(")")))
                typecast.Append(wxT(")"));
        }

        newExpr.Prepend(typecast);
    }

    s_Rect = GetScreenRect();
    HideDialog();
    m_debugger->CreateVariableObject(newExpr, false, DBG_USERR_QUICKWACTH);
}

void CLPersistentDebuggerTip::Save() const
{
    const wxPopupWindow* const puw = Get();
    const wxSize size = puw->GetSize();
    SaveValue("w", size.x);
    SaveValue("h", size.y);
}

bool CLPersistentDebuggerTip::Restore()
{
    wxPopupWindow* const puw = Get();

    long w(-1), h(-1);
    const bool hasSize = RestoreValue("w", &w) && RestoreValue("h", &h);

    if(hasSize)
        puw->SetSize(w, h);

    return hasSize;
}

inline wxPersistentObject* wxCreatePersistentObject(wxPopupWindow* puw) { return new CLPersistentDebuggerTip(puw); }
