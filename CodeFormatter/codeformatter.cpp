//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatter.cpp
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
#include "codeformatter.h"

#include "JSON.h"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "clEditorConfig.h"
#include "clEditorStateLocker.h"
#include "clFilesCollector.h"
#include "clKeyboardManager.h"
#include "clSTCLineKeeper.h"
#include "clWorkspaceManager.h"
#include "codeformatterdlg.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "macros.h"
#include "precompiled_header.h"
#include "procutils.h"
#include "workspace.h"

#include <algorithm>
#include <thread>
#include <wx/app.h> //wxInitialize/wxUnInitialize
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/xrc/xmlres.h>

namespace
{
int ID_TOOL_SOURCE_CODE_FORMATTER = ::wxNewId();

//------------------------------------------------------------------------
CodeFormatter* theFormatter = NULL;

JSONItem json_get_formatter_object(JSON* root, const wxString& formatter_name)
{
    auto json = root->toElement();
    if(!json.hasNamedObject("tools")) {
        return JSONItem{ nullptr };
    }
    auto servers = json["tools"];
    int count = servers.arraySize();
    for(int i = 0; i < count; ++i) {
        auto tool = servers[i];
        if(tool["name"].toString() != formatter_name) {
            continue;
        }
        return tool;
    }
    return JSONItem{ nullptr };
}
} // namespace

// Allocate the code formatter on the heap, it will be freed by
// the application
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(theFormatter == 0) {
        theFormatter = new CodeFormatter(manager);
    }
    return theFormatter;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor("Eran Ifrah");
    info.SetName("Source Code Formatter");
    info.SetDescription(_("Source Code Formatter (Supports C/C++/Obj-C/JavaScript/PHP files)"));
    info.SetVersion("v2.0");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeFormatter::CodeFormatter(IManager* manager)
    : IPlugin(manager)
{
    m_manager.Load();
    m_remoteHelper.reset(new CodeLiteRemoteHelper);

    m_longName = _("Source Code Formatter");
    m_shortName = _("Source Code Formatter");

    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnFormatProject, this, ID_TOOL_SOURCE_CODE_FORMATTER);
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnFormatFiles, this, XRCID("format_files"));

    EventNotifier::Get()->Bind(wxEVT_FORMAT_STRING, &CodeFormatter::OnFormatString, this);
    EventNotifier::Get()->Bind(wxEVT_FORMAT_FILE, &CodeFormatter::OnFormatFile, this);
    EventNotifier::Get()->Bind(wxEVT_BEFORE_EDITOR_SAVE, &CodeFormatter::OnBeforeFileSave, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &CodeFormatter::OnContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &CodeFormatter::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &CodeFormatter::OnWorkspaceClosed, this);

    clKeyboardManager::Get()->AddAccelerator(
        _("Source Code Formatter"),
        { { "format_source", _("Format Current Source"), "Ctrl-I" }, { "formatter_options", _("Options...") } });
}

CodeFormatter::~CodeFormatter() {}

void CodeFormatter::CreateToolBar(clToolBar* toolbar)
{
    clBitmapList* images = toolbar->GetBitmapsCreateIfNeeded();
    toolbar->AddSpacer();
    toolbar->AddTool(XRCID("format_source"), _("Format Source"), images->Add("format"), _("Format Source Code"));
    toolbar->AddTool(XRCID("formatter_options"), _("Format Options"), images->Add("cog"),
                     _("Source Code Formatter Options..."));
    // Connect the events to us

    // format the current editor
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnFormatEditor, this, XRCID("format_source"));
    m_mgr->GetTheApp()->Bind(wxEVT_UPDATE_UI, &CodeFormatter::OnFormatEditorUI, this, XRCID("format_source"));
    // open the settings dialog
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnSettings, this, XRCID("formatter_options"));
    m_mgr->GetTheApp()->Bind(wxEVT_UPDATE_UI, &CodeFormatter::OnSettingsUI, this, XRCID("formatter_options"));
}

void CodeFormatter::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("format_source"), _("Format Current Source"), _("Format Current Source"),
                          wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("formatter_options"), _("Options..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Source Code Formatter"), menu);
}

void CodeFormatter::OnFormatEditor(wxCommandEvent& e)
{
    IEditor* editor{ nullptr };
    const wxString& fileToFormat = e.GetString();

    // If we got a file name in the event, use it instead of the active editor
    if(!fileToFormat.empty()) {
        editor = m_mgr->FindEditor(fileToFormat);
    } else {
        editor = m_mgr->GetActiveEditor();
    }

    // get the editor that requires formatting
    if(!editor) {
        return;
    }
    DoFormatEditor(editor);
}

bool CodeFormatter::DoFormatEditor(IEditor* editor)
{
    // sanity
    bool is_remote = editor->IsRemoteFile();
    auto f = m_manager.GetFormatter(editor->GetRemotePathOrLocal());
    if(!f) {
        return false;
    }

    // When an inline formatter is used, prompt the user to save the file before
    if(editor->IsEditorModified()) {
        editor->Save();
    }

    wxString output;
    wxString file_path = editor->GetRemotePathOrLocal();
    bool res = false;
    if(is_remote) {
        res = f->FormatRemoteFile(file_path, FileExtManager::GetType(file_path), &output);
    } else {
        res = f->FormatFile(file_path, FileExtManager::GetType(file_path), &output);
    }

    if(!res) {
        return false;
    }

    if(f->IsInlineFormatter()) {
        // reload the current editor
        editor->ReloadFromDisk(true);
    } else {
        clEditorStateLocker locker{ editor->GetCtrl() };
        editor->GetCtrl()->SetText(output);
        m_mgr->SetStatusMessage(_("Done"), 0);
    }
    return true;
}

bool CodeFormatter::DoFormatString(const wxString& content, const wxString& fileName, wxString* output)
{
    if(content.empty()) {
        return false;
    }

    auto formatter = m_manager.GetFormatter(fileName);
    if(!formatter) {
        clDEBUG() << "Could not find suitable formatter for file:" << fileName << endl;
        return false;
    }
    return formatter->FormatString(content, fileName, output);
}

void CodeFormatter::ReloadCurrentEditor()
{
    // reload the current editor
    wxCommandEvent reload_event{ wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT };
    EventNotifier::Get()->ProcessEvent(reload_event);
}

bool CodeFormatter::DoFormatFile(const wxString& fileName, bool is_remote_format)
{
    auto f = m_manager.GetFormatter(fileName);
    if(!f) {
        clDEBUG() << "Could not find suitable formatter for file:" << fileName << endl;
        return false;
    }

    wxString output;
    bool ok = false;
    if(is_remote_format) {
        ok = f->FormatRemoteFile(fileName, FileExtManager::GetType(fileName), &output);
    } else {
        ok = f->FormatFile(fileName, FileExtManager::GetType(fileName), &output);
    }

    if(!ok) {
        return false;
    }

    auto editor = clGetManager()->FindEditor(fileName);
    if(editor) {
        if(f->IsInlineFormatter()) {
            // need to update the file itself
            editor->ReloadFromDisk(true);
        } else {
            clEditorStateLocker locker{ editor->GetCtrl() };
            editor->GetCtrl()->SetText(output);
        }
    } else {
        // the file is not opened in the CodeLite
        if(wxFileExists(fileName)) {
            FileUtils::WriteFileContent(fileName, output);
        }
    }
    return true;
}

void CodeFormatter::OnSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);

    CodeFormatterDlg dlg(EventNotifier::Get()->TopFrame(), m_manager);
    if(dlg.ShowModal() == wxID_OK) {
        // save the changes to the file system
        m_manager.Save();
    } else {
        // reload all changes from the file system
        m_manager.Load();
    }
}

void CodeFormatter::OnFormatEditorUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(m_mgr->GetActiveEditor() != NULL);
}

void CodeFormatter::OnSettingsUI(wxUpdateUIEvent& e) { e.Enable(true); }

void CodeFormatter::OnContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    event.GetMenu()->Append(XRCID("format_files"), _("Source Code Formatter"));
    m_selectedFolder = event.GetPath();
}

void CodeFormatter::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(type);
    wxUnusedVar(menu);

    if(type == MenuTypeFileView_Project) {
        // Project context menu
        menu->Prepend(ID_TOOL_SOURCE_CODE_FORMATTER, _("Source Code Formatter"));
    }
}

void CodeFormatter::UnPlug()
{
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnFormatEditor, this, XRCID("format_source"));
    m_mgr->GetTheApp()->Unbind(wxEVT_UPDATE_UI, &CodeFormatter::OnFormatEditorUI, this, XRCID("format_source"));
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnSettings, this, XRCID("formatter_options"));
    m_mgr->GetTheApp()->Unbind(wxEVT_UPDATE_UI, &CodeFormatter::OnSettingsUI, this, XRCID("formatter_options"));

    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnFormatProject, this, ID_TOOL_SOURCE_CODE_FORMATTER);
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnFormatFiles, this, XRCID("format_files"));

    EventNotifier::Get()->Unbind(wxEVT_FORMAT_STRING, &CodeFormatter::OnFormatString, this);
    EventNotifier::Get()->Unbind(wxEVT_FORMAT_FILE, &CodeFormatter::OnFormatFile, this);
    EventNotifier::Get()->Unbind(wxEVT_BEFORE_EDITOR_SAVE, &CodeFormatter::OnBeforeFileSave, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &CodeFormatter::OnContextMenu, this);
}

IManager* CodeFormatter::GetManager() { return m_mgr; }

void CodeFormatter::OnFormatString(clSourceFormatEvent& e)
{
    wxString output;
    if(DoFormatString(e.GetInputString(), e.GetFileName(), &output)) {
        e.SetFormattedString(output);
    } else {
        e.SetFormattedString(wxEmptyString);
    }
}

void CodeFormatter::OnFormatFile(clSourceFormatEvent& e)
{
    e.Skip(false);
    if(!DoFormatFile(e.GetFileName(), false)) {
        // let someone else handle this
        e.Skip();
    }
}

void CodeFormatter::OnFormatFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clGetManager()->SetStatusMessage(_("Code Formatter: scanning for files..."));
    std::thread thr(
        [=](const wxString& rootFolder, CodeFormatter* formatter) {
            clFilesScanner fs;
            std::vector<wxFileName> files;
            fs.Scan(rootFolder, files, "*", "*.o;*.obj;*.dll;*.a;*.exe;*.dylib;*.db", "build-*;.codelite;.git;.svn");

            std::vector<wxString> arrfiles;
            arrfiles.reserve(files.size());
            for(const wxFileName& f : files) {
                if(m_manager.CanFormat(f.GetFullName())) {
                    arrfiles.push_back(f.GetFullPath());
                }
            }
            formatter->CallAfter(&CodeFormatter::OnScanFilesCompleted, arrfiles);
        },
        m_selectedFolder, this);
    thr.detach();
}

void CodeFormatter::OnFormatProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    TreeItemInfo selectedItem = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(selectedItem.m_itemType != ProjectItem::TypeProject) {
        return;
    }

    ProjectPtr pProj = clCxxWorkspaceST::Get()->GetProject(selectedItem.m_text);
    CHECK_PTR_RET(pProj);

    const Project::FilesMap_t& allFiles = pProj->GetFiles();

    std::vector<wxString> filesToFormat;
    filesToFormat.reserve(allFiles.size());

    for(const auto& vt : allFiles) {
        if(m_manager.CanFormat(vt.second->GetFilename())) {
            filesToFormat.push_back(vt.second->GetFilename());
        }
    }
    BatchFormat(filesToFormat, false);
}

void CodeFormatter::BatchFormat(const std::vector<wxString>& files, bool silent)
{
    if(files.empty()) {
        if(!silent) {
            ::wxMessageBox(_("Project contains no supported files"));
        }
        return;
    }

    wxProgressDialog* dlg = nullptr;
    if(!silent) {
        wxString msg;
        msg << _("You are about to beautify ") << files.size() << _(" files\nContinue?");
        if(wxYES != ::wxMessageBox(msg, _("Source Code Formatter"), wxYES_NO | wxCANCEL | wxCENTER)) {
            return;
        }

        dlg = new wxProgressDialog(_("Source Code Formatter"), _("Formatting files..."), (int)files.size(),
                                   m_mgr->GetTheApp()->GetTopWindow());
    }
    for(size_t i = 0; i < files.size(); ++i) {
        wxString msg;
        msg << "[ " << i << " / " << files.size() << " ] " << files[i];
        if(dlg) {
            dlg->Update(i, msg);
        }
        DoFormatFile(files[i], false);
    }

    if(dlg) {
        dlg->Destroy();
    }
    EventNotifier::Get()->PostReloadExternallyModifiedEvent(false);
}

void CodeFormatter::OnBeforeFileSave(clCommandEvent& e) { e.Skip(); }

void CodeFormatter::OnScanFilesCompleted(const std::vector<wxString>& files) { BatchFormat(files, false); }

void CodeFormatter::OnWorkspaceLoaded(clWorkspaceEvent& e)
{
    e.Skip();
    m_remoteHelper->ProcessEvent(e);
    auto config = m_remoteHelper->GetPluginConfig("Source Code Formatter");

    // reset the remote commands from the previous workspace
    m_manager.ClearRemoteCommands();

    if(!config) {
        return;
    }

    wxArrayString all_tools;
    m_manager.GetAllNames(&all_tools);

    // update the formatters with the remote command template
    // note: we do not replace macros here since some of them
    // might change per activation (e.g. $(CurrentFileRelPath))
    for(size_t i = 0; i < all_tools.size(); ++i) {
        const wxString& tool_name = all_tools[i];
        auto json = json_get_formatter_object(config, tool_name);
        if(!json.isOk() || (m_manager.GetFormatterByName(tool_name) == nullptr)) {
            continue;
        }

        wxString cmd = json["command"].toString();
        wxString wd = json["working_directory"].toString();

        wxString remote_cmd;
        if(m_remoteHelper->BuildRemoteCommand(cmd, {}, wd, &remote_cmd)) {
            m_manager.GetFormatterByName(tool_name)->SetRemoteCommand(remote_cmd);
        }
    }
}

void CodeFormatter::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    m_remoteHelper->ProcessEvent(e);
}
