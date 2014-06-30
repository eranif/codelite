//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : outline_tab.cpp
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

#include "outline_tab.h"
#include "outline_symbol_tree.h"
#include "event_notifier.h"
#include "plugin.h"
#include <wx/stc/stc.h>
#include <wx/menu.h>

const wxEventType wxEVT_SV_GOTO_DEFINITION  = wxNewEventType();
const wxEventType wxEVT_SV_GOTO_DECLARATION = wxNewEventType();
const wxEventType wxEVT_SV_FIND_REFERENCES  = wxNewEventType();
const wxEventType wxEVT_SV_RENAME_SYMBOL    = wxNewEventType();
const wxEventType wxEVT_SV_OPEN_FILE        = wxNewEventType();

OutlineTab::OutlineTab(wxWindow* parent, IManager* mgr)
    : OutlineTabBaseClass(parent)
    , m_mgr(mgr)
{
    m_tree = new svSymbolTree(this, m_mgr, wxID_ANY);
    m_tree->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OutlineTab::OnMenu), NULL, this);
    
    m_tree->AssignImageList( svSymbolTree::CreateSymbolTreeImages() );

    GetSizer()->Add(m_tree, 1, wxEXPAND);

    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(OutlineTab::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OutlineTab::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OutlineTab::OnEditorClosed), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OutlineTab::OnAllEditorsClosed), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(OutlineTab::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(OutlineTab::OnFilesTagged), NULL, this);
    
    Connect(wxEVT_SV_GOTO_DEFINITION,  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    Connect(wxEVT_SV_GOTO_DECLARATION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    Connect(wxEVT_SV_FIND_REFERENCES,  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    Connect(wxEVT_SV_RENAME_SYMBOL,    wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    m_themeHelper = new ThemeHandlerHelper(this);
    
}

OutlineTab::~OutlineTab()
{
    wxDELETE(m_themeHelper);
    m_tree->Disconnect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OutlineTab::OnMenu), NULL, this);
    
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(OutlineTab::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OutlineTab::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OutlineTab::OnEditorClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OutlineTab::OnAllEditorsClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(OutlineTab::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(OutlineTab::OnFilesTagged), NULL, this);

    Disconnect(wxEVT_SV_GOTO_DEFINITION,  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    Disconnect(wxEVT_SV_GOTO_DECLARATION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    Disconnect(wxEVT_SV_FIND_REFERENCES,  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    Disconnect(wxEVT_SV_RENAME_SYMBOL,    wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
}

void OutlineTab::OnSearchSymbol(wxCommandEvent& event)
{
    event.Skip();
    wxString name = m_textCtrlSearch->GetValue();
    name.Trim().Trim(false);
    m_tree->SelectItemByName(name);
}

void OutlineTab::OnSearchEnter(wxCommandEvent& event)
{
    event.Skip();
    wxString name = m_textCtrlSearch->GetValue();
    name.Trim().Trim(false);
    if(name.IsEmpty() == false) {
        m_tree->ActivateSelectedItem();
    }
}

void OutlineTab::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    if(editor) {
        m_tree->BuildTree(editor->GetFileName());
    }
}

void OutlineTab::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    m_tree->Clear();
}

void OutlineTab::OnEditorClosed(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    if (editor && m_tree->GetFilename() == editor->GetFileName()) {
        m_tree->Clear();
        m_tree->ClearCache();
    }
}

void OutlineTab::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_tree->Clear();
}

void OutlineTab::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
}

void OutlineTab::OnFilesTagged(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    if( editor ) {
        m_tree->BuildTree( editor->GetFileName() );
        
        if(editor->GetSTC()) {
            // make sure we dont steal the focus from the editor...
            editor->GetSTC()->SetFocus();
        }
        
    } else {
        m_tree->Clear();
    }
}
void OutlineTab::OnMenu(wxContextMenuEvent& e)
{
    wxMenu menu;
    
    if ( IsIncludeFileNode() ) {
        menu.Append(wxEVT_SV_OPEN_FILE, _("Open..."));
        menu.Connect(wxEVT_SV_OPEN_FILE,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutlineTab::OnOpenFile), NULL, this);
        
    } else {
        menu.Append(wxEVT_SV_GOTO_DECLARATION, _("Goto Declaration"));
        menu.Append(wxEVT_SV_GOTO_DEFINITION,  _("Goto Implementation"));
        menu.AppendSeparator();
        menu.Append(wxEVT_SV_FIND_REFERENCES , _("Find References..."));
        menu.AppendSeparator();
        menu.Append(wxEVT_SV_RENAME_SYMBOL , _("Rename Symbol..."));
        
        menu.Connect(wxEVT_SV_GOTO_DEFINITION,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutlineTab::OnGotoImpl),      NULL, this);
        menu.Connect(wxEVT_SV_GOTO_DECLARATION, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutlineTab::OnGotoDecl),      NULL, this);
        menu.Connect(wxEVT_SV_FIND_REFERENCES , wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutlineTab::OnFindReferenes), NULL, this);
        menu.Connect(wxEVT_SV_RENAME_SYMBOL ,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutlineTab::OnRenameSymbol),  NULL, this);
    }
    
    m_tree->PopupMenu(&menu);
}

void OutlineTab::OnGotoDecl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_decl"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnGotoImpl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_impl"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnFindReferenes(wxCommandEvent& e)
{    
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_references"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnRenameSymbol(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("rename_symbol"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnItemSelectedUI(wxUpdateUIEvent& e)
{
    IEditor *editor = m_mgr->GetActiveEditor();
    e.Enable(editor && editor->GetSelection().IsEmpty() == false);
}

bool OutlineTab::IsIncludeFileNode()
{
    return m_tree->IsSelectedItemIncludeFile();
}

void OutlineTab::OnOpenFile(wxCommandEvent& e)
{
    wxString includedFile = m_tree->GetSelectedIncludeFile();
    if(includedFile.IsEmpty())
        return;
    
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("open_include_file"));
    evt.SetString(includedFile);
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

