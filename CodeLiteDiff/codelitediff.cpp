//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : codelitediff.cpp
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

#include "DiffSideBySidePanel.h"
#include "NewFileComparison.h"
#include "clDiffFrame.h"
#include "clKeyboardManager.h"
#include "codelitediff.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "macros.h"
#include "wx/menu.h"
#include <wx/ffile.h>
#include <wx/xrc/xmlres.h>

static CodeLiteDiff* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new CodeLiteDiff(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Diff Plugin"));
    info.SetDescription(_("CodeLite Diff Plugin"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeLiteDiff::CodeLiteDiff(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("CodeLite Diff Plugin");
    m_shortName = wxT("Diff Plugin");

    Bind(wxEVT_MENU, &CodeLiteDiff::OnDiff, this, XRCID("diff_compare_with"));
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_TAB_LABEL, &CodeLiteDiff::OnTabContextMenu, this);
    clKeyboardManager::Get()->AddGlobalAccelerator("diff_new_comparison", "Ctrl-Shift-C",
                                                   "Plugins::Diff Tool::New File Comparison");
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &CodeLiteDiff::OnNewDiff, this, XRCID("diff_new_comparison"));
}

CodeLiteDiff::~CodeLiteDiff() {}

clToolBar* CodeLiteDiff::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void CodeLiteDiff::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("diff_new_comparison"), _("New Diff.."), _("Start new diff"));
    pluginsMenu->Append(wxID_ANY, _("Diff Tool"), menu);
}

void CodeLiteDiff::UnPlug()
{
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &CodeLiteDiff::OnNewDiff, this, XRCID("diff_new_comparison"));
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_TAB_LABEL, &CodeLiteDiff::OnTabContextMenu, this);
    Unbind(wxEVT_MENU, &CodeLiteDiff::OnDiff, this, XRCID("diff_compare_with"));
}

void CodeLiteDiff::OnNewDiff(wxCommandEvent& e)
{
    clDiffFrame* diff = new clDiffFrame(EventNotifier::Get()->TopFrame());
    diff->Show();
}

void CodeLiteDiff::OnTabContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    DoClear();
    IEditor* activeEditor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(activeEditor);

    m_leftFile = activeEditor->GetFileName();

    // Edit the context menu
    wxMenuItem* mi = new wxMenuItem(event.GetMenu(), XRCID("diff_compare_with"), _("Compare with..."), "");
    mi->SetBitmap(m_mgr->GetStdIcons()->LoadBitmap("diff"));
    event.GetMenu()->AppendSeparator();
    event.GetMenu()->Append(mi);
    event.GetMenu()->Bind(wxEVT_MENU, &CodeLiteDiff::OnDiff, this, XRCID("diff_compare_with"));
}

void CodeLiteDiff::DoClear() { m_leftFile.Clear(); }

void CodeLiteDiff::OnDiff(wxCommandEvent& event)
{
    bool tempfile(false);
    NewFileComparison dlg(EventNotifier::Get()->TopFrame(), m_leftFile);
    if(dlg.ShowModal() == wxID_OK) {
        if(m_leftFile.GetName().StartsWith("Untitled")) {
            tempfile = true;
            m_leftFile = SaveEditorToTmpfile(m_mgr->GetActiveEditor());
            if(!m_leftFile.IsOk()) {
                CL_DEBUG("CodeLiteDiff::OnDiff: call to SaveEditorToTmpfile() failed for m_leftFile");
                return;
            }
        }
        wxString secondFile = dlg.GetTextCtrlFileName()->GetValue();
        if(secondFile.StartsWith("Untitled")) {
            tempfile = true;
            IEditor* editor = m_mgr->FindEditor(secondFile);
            if(!editor) {
                CL_DEBUG("CodeLiteDiff::OnDiff: call to FindEditor() failed");
                return;
            }
            wxFileName rightFn = SaveEditorToTmpfile(editor);
            if(!rightFn.IsOk()) {
                CL_DEBUG("CodeLiteDiff::OnDiff: call to SaveEditorToTmpfile() failed for secondFile");
                return;
            }
            secondFile = rightFn.GetFullPath();
        }

        // Check that we're not trying to diff an editor against itself
        // If we are and it's been edited, diff against the unaltered version
        if(m_leftFile.GetFullPath() == secondFile) {
            IEditor* editor = m_mgr->FindEditor(secondFile);
            if(editor && editor->IsModified()) {
                wxFileName rightFn = SaveEditorToTmpfile(editor);
                if(!rightFn.IsOk()) {
                    CL_DEBUG("CodeLiteDiff::OnDiff: call to SaveEditorToTmpfile() failed for secondFile");
                    return;
                }
                secondFile = rightFn.GetFullPath();

            } else {
                CL_DEBUG("CodeLiteDiff::OnDiff: trying to diff an editor against itself");
                return;
            }
        }

        clDiffFrame* diffView = new clDiffFrame(EventNotifier::Get()->TopFrame(), m_leftFile, secondFile, tempfile);
        diffView->Show();
    }
}

wxFileName CodeLiteDiff::SaveEditorToTmpfile(IEditor* editor) const
{
    wxString content = editor->GetEditorText();
    if(content.empty()) {
        return wxFileName(); // Nothing to diff
    }

    wxString tpath(wxFileName::GetTempDir());
    tpath << wxFileName::GetPathSeparator() << "CLdiff" << wxFileName::GetPathSeparator();
    wxFileName::Mkdir(tpath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    wxFileName tmpFile(wxFileName::CreateTempFileName(tpath + editor->GetFileName().GetName()));
    if(!tmpFile.IsOk()) {
        return wxFileName();
    }

    tmpFile.SetExt(editor->GetFileName().GetExt());
    wxFFile fp(tmpFile.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(content);
        fp.Close();
    } else {
        return wxFileName();
    }

    return tmpFile;
}