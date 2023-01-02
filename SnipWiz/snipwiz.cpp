//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : snipwiz.cpp
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

/////////////////////////////////////////////////////////////////////////////
// Name:        snipwiz.cpp
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     09/02/08
// RCS-ID:
// Copyright:   2008 Frank Lichtner
// Licence:		GNU General Public Licence
/////////////////////////////////////////////////////////////////////////////
#include "event_notifier.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "editsnippetsdlg.h"
#include "snipwiz.h"
#include "swGlobals.h"
#include "templateclassdlg.h"
#include "workspace.h"
#include "wxSerialize.h"
#include <wx/clipbrd.h>
#include <wx/filename.h>
#include <wx/numdlg.h>
#include <wx/textdlg.h>
#include <wx/wfstream.h>
//------------------------------------------------------------

#define FRLSNIPWIZ_VERSION 1000
#define FRLSNIPWIZ_HEADER _T("Code snippet wizard file")

#define CARET wxT("@")
#define USER_ESC_CARET wxT("\\@")
#define SELECTION wxT("$")
#define USER_ESC_SELECTION wxT("\\$")
#define REAL_CARET_STR wxT("%CARET%")
#define REAL_SELECTION_STR wxT("%SELECTION%")
#define TMP_ESC_CARET_STR wxT("%ESC_CARET%")
#define TMP_ESC_SELECTION_STR wxT("%ESC_SELECTION%")

////------------------------------------------------------------
static SnipWiz* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new SnipWiz(manager);
    }
    return thePlugin;
}
//------------------------------------------------------------

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Frank Lichtner"));
    info.SetName(plugName);
    info.SetDescription(_("A small tool to add expandable code snippets and template classes"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}
//------------------------------------------------------------
CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

//------------------------------------------------------------
// When creating new file, we default the EOL mode to
// the OS conventions
static int GetEOLByOS()
{
#if defined(__WXMAC__)
    return 1 /*wxSCI_EOL_CR*/;
#elif defined(__WXGTK__)
    return 2 /*wxSCI_EOL_LF*/;
#else
    return 0 /*wxSCI_EOL_CRLF*/;
#endif
}

//------------------------------------------------------------
SnipWiz::SnipWiz(IManager* manager)
    : IPlugin(manager)
{
    m_topWin = NULL;
    m_longName = _("Snippet wizard");
    m_shortName = plugName;
    m_topWin = m_mgr->GetTheApp();

    // get config path
    m_configPath = clStandardPaths::Get().GetUserDataDir();
    m_configPath += wxFILE_SEP_PATH;
    m_configPath += wxT("config");
    m_configPath += wxFILE_SEP_PATH;

    m_modified = false;
    m_StringDb.SetCompress(true);

    if(!m_StringDb.Load(m_configPath + defaultTmplFile)) {
        // For compatibility with CodeLite < 15.0.6:
        // we don't use this directory for storing templates anymore.
        wxString pluginPath = m_mgr->GetStartupDirectory();
        pluginPath += wxFILE_SEP_PATH;
        pluginPath += wxT("templates");
        pluginPath += wxFILE_SEP_PATH;
        if(m_StringDb.Load(pluginPath + defaultTmplFile)) {
            m_modified = true;
        }
    }

    m_StringDb.GetAllSnippetKeys(m_snippets);
    if(!m_snippets.GetCount()) {
        IntSnippets();
        m_StringDb.GetAllSnippetKeys(m_snippets);
    }
    m_snippets.Sort();
    m_clipboard.Empty();
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &SnipWiz::OnEditorContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &SnipWiz::OnFolderContextMenu, this);
}

//------------------------------------------------------------
SnipWiz::~SnipWiz()
{
    if(m_modified)
        m_StringDb.Save(m_configPath + defaultTmplFile);
}
//------------------------------------------------------------

void SnipWiz::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }
//------------------------------------------------------------

void SnipWiz::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, IDM_SETTINGS, _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(menu, IDM_CLASS_WIZ, _("Template class..."), _("Template class..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, _("SnipWiz"), menu);

    m_topWin->Connect(IDM_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SnipWiz::OnSettings), NULL,
                      this);
    m_topWin->Connect(IDM_CLASS_WIZ, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SnipWiz::OnClassWizard), NULL,
                      this);

    m_topWin->Connect(IDM_EXP_SWITCH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SnipWiz::OnMenuExpandSwitch),
                      NULL, this);
    m_topWin->Connect(IDM_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SnipWiz::OnMenuPaste), NULL, this);
    AttachDynMenus();
}

//------------------------------------------------------------
void SnipWiz::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Folder) {
        // Create the popup menu for the virtual folders
        wxMenuItem* item(NULL);

        item = new wxMenuItem(menu, wxID_SEPARATOR);
        menu->Prepend(item);
        m_vdDynItems.push_back(item);

        item = new wxMenuItem(menu, IDM_CLASS_WIZ, _("New Class from Template..."), wxEmptyString, wxITEM_NORMAL);
        menu->Prepend(item);
        m_vdDynItems.push_back(item);
    }
}

//------------------------------------------------------------

void SnipWiz::UnPlug()
{
    m_topWin->Disconnect(IDM_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SnipWiz::OnSettings), NULL,
                         this);
    m_topWin->Disconnect(IDM_CLASS_WIZ, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SnipWiz::OnClassWizard),
                         NULL, this);

    m_topWin->Disconnect(IDM_EXP_SWITCH, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(SnipWiz::OnMenuExpandSwitch), NULL, this);
    m_topWin->Disconnect(IDM_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SnipWiz::OnMenuPaste), NULL,
                         this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &SnipWiz::OnFolderContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &SnipWiz::OnEditorContextMenu, this);
    DetachDynMenus();
}

//------------------------------------------------------------
void SnipWiz::OnMenuExpandSwitch(wxCommandEvent& e)
{
    IEditor* editor = GetEditor();
    if(!editor)
        return;

    bool isSelection = false;
    wxString var = editor->GetSelection();
    if(!var.IsEmpty())
        isSelection = true;
    var = ::wxGetTextFromUser(_("Enter identifier name"), _("switch(...)"), var);
    if(var.IsEmpty())
        return;
    long count = ::wxGetNumberFromUser(_("Enter number of cases"), _("Cases:"), _("switch(...)"), 1, 1, 20);
    if(count < 1)
        return;

    int curEol = editor->GetEOL();
    int curPos = editor->GetCurrentPosition();
    wxString tabs = GetTabs(editor, curPos);

    wxString output = wxString::Format(wxT("switch( %s )%s%s{%s"), var.c_str(), eol[curEol].c_str(), tabs.c_str(),
                                       eol[curEol].c_str());
    for(long i = 0; i < count; i++)
        output += wxString::Format(wxT("%scase :%s%sbreak;%s"), tabs.c_str(), eol[curEol].c_str(), tabs.c_str(),
                                   eol[curEol].c_str());

    output += tabs.c_str();
    output += wxT("}");
    if(isSelection)
        editor->ReplaceSelection(output);
    else
        editor->InsertText(curPos, output);
}

//------------------------------------------------------------
void SnipWiz::OnMenuSnippets(wxCommandEvent& e)
{
    IEditor* editor = GetEditor();
    if(!editor)
        return;

    bool crtl = ::wxGetKeyState(WXK_CONTROL);
    bool sourceIsMenu(false);

    wxMenu* m = dynamic_cast<wxMenu*>(e.GetEventObject());
    if(m) {
        sourceIsMenu = true;
    }

    if(e.GetId() >= IDM_ADDSTART && e.GetId() < (IDM_ADDSTART + (int)m_snippets.GetCount())) {
        wxString key = m_snippets.Item(e.GetId() - IDM_ADDSTART);
        wxString srText = m_StringDb.GetSnippetString(key);
        wxString selection = editor->GetSelection();

        // replace template eols with current
        int curEol = editor->GetEOL();
        if(srText.Find(eol[2]) != wxNOT_FOUND)
            srText.Replace(eol[2], eol[curEol].c_str());

        // Replace any escaped carets/selection strings
        srText.Replace(USER_ESC_CARET, TMP_ESC_CARET_STR);
        srText.Replace(USER_ESC_SELECTION, TMP_ESC_SELECTION_STR);
        srText.Replace(CARET, REAL_CARET_STR);
        srText.Replace(SELECTION, REAL_SELECTION_STR);

        // selection ?
        if(srText.Find(REAL_SELECTION_STR) != wxNOT_FOUND)
            srText.Replace(REAL_SELECTION_STR, selection.c_str());

        // restore the escaped selection, this time without the escaping backslash
        srText.Replace(TMP_ESC_SELECTION_STR, SELECTION);

        // restore the escaped caret, this time without the escaping backslash
        srText.Replace(TMP_ESC_CARET_STR, CARET);

        // if the user pressed control while clicking
        if(crtl && sourceIsMenu) {
            m_clipboard = srText;
            // remove caret mark if there
            srText.Replace(REAL_CARET_STR, wxT(""));

            // copy text to clipboard
            if(wxTheClipboard->Open()) {
                wxTheClipboard->SetData(new wxTextDataObject(srText));
                wxTheClipboard->Close();
            }
        } else {
            // otherwise insert text

            // format the text for insertion
            wxString output = FormatOutput(editor, srText);

            int curPos = editor->GetCurrentPosition();
            if(selection.Len() != 0) {
                curPos = editor->GetSelectionStart();
            }

            // get caret position
            long cursorPos = output.Find(REAL_CARET_STR);
            if(cursorPos != wxNOT_FOUND)
                output.Remove(cursorPos, wxStrlen(REAL_CARET_STR));
            editor->ReplaceSelection(output);
            // set caret
            if(cursorPos != wxNOT_FOUND)
                editor->SetCaretAt(curPos + cursorPos);
            else
                editor->SetCaretAt(curPos + output.Len());
        }
    }
}
//------------------------------------------------------------
void SnipWiz::OnMenuPaste(wxCommandEvent& e)
{
    wxUnusedVar(e);
    IEditor* editor = GetEditor();
    if(!editor)
        return;

    if(m_clipboard.IsEmpty())
        return;
    // otherwise insert text
    wxString output = FormatOutput(editor, m_clipboard);
    wxString selection = editor->GetSelection();
    int curPos = editor->GetCurrentPosition() - selection.Len();
    // get caret position
    long cursorPos = output.Find(REAL_CARET_STR);
    if(cursorPos != wxNOT_FOUND)
        output.Remove(cursorPos, wxStrlen(REAL_CARET_STR));
    editor->ReplaceSelection(output);
    // set caret
    if(cursorPos != wxNOT_FOUND)
        editor->SetCaretAt(curPos + cursorPos);
    else
        editor->SetCaretAt(curPos + output.Len());
}
//------------------------------------------------------------
wxString SnipWiz::FormatOutput(IEditor* pEditor, const wxString& text)
{
    wxString output = text;
    int curPos = pEditor->GetCurrentPosition();
    int curEol = pEditor->GetEOL();
    wxString tabs = GetTabs(pEditor, curPos);
    output.Replace(eol[curEol], eol[curEol] + tabs);
    return output;
}

//------------------------------------------------------------
wxMenu* SnipWiz::CreateSubMenu()
{
    wxMenu* parentMenu = new wxMenu();

    wxMenuItem* item(NULL);
    if(!m_clipboard.IsEmpty()) {
        item = new wxMenuItem(parentMenu, IDM_PASTE, _("Paste buffer"), _("Paste buffer"), wxITEM_NORMAL);
        parentMenu->Append(item);
        parentMenu->AppendSeparator();
    }
    item = new wxMenuItem(parentMenu, IDM_EXP_SWITCH, _("switch{...}"), _("switch{...}"), wxITEM_NORMAL);
    parentMenu->Append(item);
    parentMenu->AppendSeparator();

    for(wxUint32 i = 0; i < m_snippets.GetCount(); i++) {
        item = new wxMenuItem(parentMenu, IDM_ADDSTART + i, m_snippets.Item(i), m_snippets.Item(i), wxITEM_NORMAL);
        parentMenu->Append(item);
    }

    return parentMenu;
}
//------------------------------------------------------------
// opens settings
void SnipWiz::OnSettings(wxCommandEvent& e)
{
    EditSnippetsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), this, m_mgr);
    dlg.ShowModal();

    if(dlg.GetModified()) {
        m_snippets.Clear();
        m_StringDb.GetAllSnippetKeys(m_snippets);
        m_snippets.Sort();
        DetachDynMenus();
        AttachDynMenus();
        m_modified = true;
    }
}

//------------------------------------------------------------
// generate some default entries
void SnipWiz::IntSnippets()
{
    m_StringDb.SetSnippetString(wxT("//-"), wxT("//------------------------------------------------------------@"));
    m_StringDb.SetSnippetString(wxT("wxT(\""), wxT("wxT(\"@\")"));
    m_StringDb.SetSnippetString(wxT("wxT($"), wxT("wxT( $ )"));
    m_StringDb.SetSnippetString(wxT("if($"), wxT("if ( $ )\n{\n\t@\n}"));
    m_StringDb.SetSnippetString(wxT("whi($"), wxT("while ( $ )\n{\n\t@\n}"));
    m_StringDb.SetSnippetString(wxT("($)"), wxT("( $ )@"));
    m_StringDb.SetSnippetString(wxT("{$}"), wxT("{ $ }@"));
    m_StringDb.SetSnippetString(wxT("[$]"), wxT("[ $ ]@"));
    m_StringDb.SetSnippetString(wxT("wxU($"), wxT("wxUnusedVar( $ );@"));
    m_StringDb.SetSnippetString(wxT("for($"), wxT("for( $ = 0; $  < @; $++ )"));
    m_StringDb.SetSnippetString(wxT("for(Ii"), wxT("for( int i = 0; i  < $; i++ )@"));
    m_StringDb.SetSnippetString(wxT("for(Ui"), wxT("for( unsigned int i = 0; i  < $; i++ )@"));
    m_StringDb.SetSnippetString(wxT("R\"("), wxT("R\"$(@)$\""));
}

//------------------------------------------------------------
// detach dynamic menus
void SnipWiz::DetachDynMenus()
{
    m_topWin->Disconnect(IDM_ADDSTART, IDM_ADDSTART + m_snippets.GetCount() - 1, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(SnipWiz::OnMenuSnippets), NULL, this);
}
//------------------------------------------------------------
// attach dynamic menus
void SnipWiz::AttachDynMenus()
{
    m_topWin->Connect(IDM_ADDSTART, IDM_ADDSTART + m_snippets.GetCount() - 1, wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(SnipWiz::OnMenuSnippets), NULL, this);
}
//------------------------------------------------------------
// find current indentation
long SnipWiz::GetCurrentIndentation(IEditor* pEditor, long pos)
{
    wxString buffer = pEditor->GetEditorText().Left(pos);
    long tabCount = 0;
    wxChar ch = (pEditor->GetEOL() == 1) ? wxT('\r') : wxT('\n');
    buffer = buffer.AfterLast(ch);
    for(long i = 0; i < (long)buffer.Len(); i++)
        if(buffer.GetChar(i) == wxT('\t'))
            tabCount++;

    return tabCount;
}
//------------------------------------------------------------
// generates a string with tabs for current indentation
wxString SnipWiz::GetTabs(IEditor* pEditor, long pos)
{
    long indent = GetCurrentIndentation(pEditor, pos);
    wxString tabs = wxT("");
    for(long i = 0; i < indent; i++)
        tabs += wxT("\t");
    return tabs;
}

//------------------------------------------------------------
// returns pointer to editor
IEditor* SnipWiz::GetEditor()
{
    IEditor* editor = m_mgr->GetActiveEditor();
    if(!editor) {
        ::wxMessageBox(noEditor, codeLite, wxICON_WARNING | wxOK);
        return NULL;
    }
    return editor;
}

//------------------------------------------------------------
void SnipWiz::OnClassWizard(wxCommandEvent& e)
{
    TemplateClassDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), this, m_mgr);

    dlg.SetCurEol(GetEOLByOS());
    dlg.SetConfigPath(m_configPath);
    dlg.ShowModal();
    if(dlg.GetModified()) {
        m_modified = true;
    }
}

void SnipWiz::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(FileExtManager::IsCxxFile(editor->GetFileName())) {
        wxMenu* newMenu = CreateSubMenu();
        event.GetMenu()->Append(wxID_ANY, _("Snippets"), newMenu);
    }
}

void SnipWiz::OnFolderContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    auto menu = event.GetMenu();
    wxString path = event.GetPath();
    menu->Append(XRCID("snipwiz_add_class"), _("New class from template"));
    menu->Bind(
        wxEVT_MENU,
        [this, path](wxCommandEvent& e) {
            wxUnusedVar(e);
            TemplateClassDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), this, m_mgr);
            dlg.SetCurEol(GetEOLByOS());
            dlg.SetConfigPath(m_configPath);
            dlg.SetProjectPath(path);
            dlg.ShowModal();
        },
        XRCID("snipwiz_add_class"));
}
