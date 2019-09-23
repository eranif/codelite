//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cscope.cpp
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
#include "CScopeSettingsDlg.h"
#include "bitmap_loader.h"
#include "clKeyboardManager.h"
#include "cscope.h"
#include "cscopedbbuilderthread.h"
#include "cscopestatusmessage.h"
#include "cscopetab.h"
#include "csscopeconfdata.h"
#include "dirsaver.h"
#include "event_notifier.h"
#include "exelocator.h"
#include "file_logger.h"
#include "procutils.h"
#include "workspace.h"
#include "wx/ffile.h"
#include <wx/app.h>
#include <wx/aui/framemanager.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>
#include "clFileSystemWorkspace.hpp"
#include <fileutils.h>

static Cscope* thePlugin = NULL;

static const wxString CSCOPE_NAME = _("CScope");

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new Cscope(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah, patched by Stefan Roesch"));
    info.SetName(CSCOPE_NAME);
    info.SetDescription(_("CScope Integration for CodeLite"));
    info.SetVersion(wxT("v1.1"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

Cscope::Cscope(IManager* manager)
    : IPlugin(manager)
    , m_topWindow(NULL)
{
    m_longName = _("CScope Integration for CodeLite");
    m_shortName = CSCOPE_NAME;
    m_topWindow = m_mgr->GetTheApp();

    m_cscopeWin = new CscopeTab(m_mgr->GetOutputPaneNotebook(), m_mgr);
    m_mgr->GetOutputPaneNotebook()->AddPage(m_cscopeWin, CSCOPE_NAME, false,
                                            m_mgr->GetStdIcons()->LoadBitmap("cscope"));
    m_tabHelper.reset(new clTabTogglerHelper(CSCOPE_NAME, m_cscopeWin, "", NULL));
    m_tabHelper->SetOutputTabBmp(m_mgr->GetStdIcons()->LoadBitmap("cscope"));

    Connect(wxEVT_CSCOPE_THREAD_DONE, wxCommandEventHandler(Cscope::OnCScopeThreadEnded), NULL, this);
    Connect(wxEVT_CSCOPE_THREAD_UPDATE_STATUS, wxCommandEventHandler(Cscope::OnCScopeThreadUpdateStatus), NULL, this);

    // start the helper thread
    CScopeThreadST::Get()->Start();

    // Register keyboard shortcuts for CScope
    clKeyboardManager::Get()->AddGlobalAccelerator("cscope_find_user_symbol", "Alt-)", "Plugins::CScope::Find");
    clKeyboardManager::Get()->AddGlobalAccelerator("cscope_find_symbol", "Alt-0",
                                                   "Plugins::CScope::Find selected text");
    clKeyboardManager::Get()->AddGlobalAccelerator("cscope_find_global_definition", "Alt-1",
                                                   "Plugins::CScope::Find this global definition");
    clKeyboardManager::Get()->AddGlobalAccelerator("cscope_functions_calling_this_function", "Alt-2",
                                                   "Plugins::CScope::Find functions called by this function");
    clKeyboardManager::Get()->AddGlobalAccelerator("cscope_functions_called_by_this_function", "Alt-3",
                                                   "Plugins::CScope::Find functions calling this function");
    clKeyboardManager::Get()->AddGlobalAccelerator("cscope_create_db", "Alt-4",
                                                   "Plugins::CScope::Create CScope database");
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &Cscope::OnEditorContentMenu, this);
}

Cscope::~Cscope() {}

void Cscope::CreateToolBar(clToolBar* toolbar)
{
    // support both toolbars icon size
    int size = m_mgr->GetToolbarIconSize();

    // Sample code that adds single button to the toolbar
    // and associates an image to it
    BitmapLoader* bitmapLoader = m_mgr->GetStdIcons();

    // use the large icons set
    toolbar->AddSpacer();
    toolbar->AddTool(XRCID("cscope_find_symbol"), _("Find this C symbol"), bitmapLoader->LoadBitmap("find", size),
                     _("Find this C symbol"));
    toolbar->AddTool(XRCID("cscope_functions_calling_this_function"), _("Find functions calling this function"),
                     bitmapLoader->LoadBitmap("step_in", size), _("Find functions calling this function"));
    toolbar->AddTool(XRCID("cscope_functions_called_by_this_function"), _("Find functions called by this function"),
                     bitmapLoader->LoadBitmap("step_out", size), _("Find functions called by this function"));

    // Command events
    m_topWindow->Connect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_create_db"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnCreateDB), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_settings"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnDoSettings), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_find_user_symbol"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindUserInsertedSymbol), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFunction), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_files_including_this_filename"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindFilesIncludingThisFname), NULL, (wxEvtHandler*)this);

    // UI events
    m_topWindow->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_files_including_this_filename"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_create_db"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnWorkspaceOpenUI),
                         NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_find_global_definition"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_find_symbol"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL,
                         (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_find_user_symbol"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(Cscope::OnWorkspaceOpenUI), NULL, (wxEvtHandler*)this);
}

void Cscope::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("cscope_find_user_symbol"), _("Find ..."), _("Find ..."), wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("cscope_find_symbol"), _("Find selected text"), _("Find this C symbol"),
                          wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cscope_find_global_definition"), _("Find this global definition"),
                          _("Find this C global definition"), wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cscope_functions_called_by_this_function"),
                          _("Find functions called by this function"), _("Find functions called by this function"),
                          wxITEM_NORMAL);
    menu->Append(item);

    item =
        new wxMenuItem(menu, XRCID("cscope_functions_calling_this_function"), _("Find functions calling this function"),
                       _("Find functions calling this function"), wxITEM_NORMAL);
    menu->Append(item);

    item =
        new wxMenuItem(menu, XRCID("cscope_files_including_this_filename"), _("Find files #&including this filename"),
                       _("Find files #including this filename"), wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("cscope_create_db"), _("Create CScope database"),
                          _("Create/Recreate the cscope database"), wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("cscope_settings"), _("CScope settings"), _("Configure cscope"), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, CSCOPE_NAME, menu);
}

void Cscope::UnPlug()
{
    m_tabHelper.reset(NULL);
    m_topWindow->Disconnect(XRCID("cscope_functions_called_by_this_function"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_files_including_this_filename"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_create_db"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(Cscope::OnWorkspaceOpenUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_functions_calling_this_function"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_find_global_definition"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_find_symbol"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),
                            NULL, (wxEvtHandler*)this);

    m_topWindow->Disconnect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFunction), NULL,
                            (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_files_including_this_filename"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(Cscope::OnFindFilesIncludingThisFname), NULL, (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL,
                            (wxEvtHandler*)this);
    m_topWindow->Disconnect(XRCID("cscope_create_db"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(Cscope::OnCreateDB), NULL, (wxEvtHandler*)this);

    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_cscopeWin == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_cscopeWin->Destroy();
            break;
        }
    }
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &Cscope::OnEditorContentMenu, this);
    CScopeThreadST::Get()->Stop();
    CScopeThreadST::Free();
}

//---------------------------------------------------------------------------------
wxMenu* Cscope::CreateEditorPopMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interseted is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cscope_find_symbol"), _("&Find this C symbol"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cscope_find_global_definition"), _("Find this &global definition"),
                          wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cscope_functions_called_by_this_function"),
                          _("Find functions &called by this function"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cscope_functions_calling_this_function"),
                          _("Fi&nd functions calling this function"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cscope_files_including_this_filename"),
                          _("Find files #&including this filename"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("cscope_create_db"), _("Create CScope &database"),
                          _("Create/Recreate the cscope database"), wxITEM_NORMAL);
    menu->Append(item);

    // connect the events
    m_topWindow->Connect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFunction), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
    m_topWindow->Connect(XRCID("cscope_create_db"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Cscope::OnCreateDB), NULL, (wxEvtHandler*)this);
    return menu;
}

wxString Cscope::DoCreateListFile(bool force)
{
    // get the scope
    CScopeConfData settings;
    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);

    wxArrayString tmpfiles;
    wxString privateFolder = GetWorkingDirectory();
    wxFileName list_file(privateFolder, "cscope_file.list");
    bool createFileList = force || settings.GetRebuildOption() || !list_file.FileExists();
    if(createFileList) {
        std::vector<wxFileName> files;
        if(clFileSystemWorkspace::Get().IsOpen()) {
            const std::vector<wxFileName>& all_files = clFileSystemWorkspace::Get().GetFiles();
            if(!all_files.empty()) {
                files.reserve(all_files.size());
                for(wxFileName fn : all_files) {
                    wxString ext = fn.GetExt();
                    if(ext == wxT("exe") || ext == wxT("") || ext == wxT("xpm") || ext == wxT("png")) { continue; }
                    fn.MakeRelativeTo(privateFolder);
                    files.push_back(fn);
                }
            }
        } else {
            wxArrayString projects;
            m_mgr->GetWorkspace()->GetProjectList(projects);
            wxString err_msg;
            m_cscopeWin->SetMessage(_("Creating file list..."), 5);

            if(settings.GetScanScope() == SCOPE_ENTIRE_WORKSPACE) {
                m_mgr->GetWorkspace()->GetWorkspaceFiles(tmpfiles);
            } else {
                // SCOPE_ACTIVE_PROJECT
                ProjectPtr proj = m_mgr->GetWorkspace()->GetActiveProject();
                if(proj) { proj->GetFilesAsStringArray(tmpfiles); }
            }
            // iterate over the files and convert them to be relative path
            // Also remove any .exe files (one of which managed to crash cscope),
            // and files without an ext, which may be binaries and are unlikely to be .c or .h files in disguise; and
            // .xpm and .png too
            if(!tmpfiles.empty()) {
                files.reserve(tmpfiles.size());
                for(const wxString& filepath : tmpfiles) {
                    wxFileName fn(filepath);
                    wxString ext = fn.GetExt();
                    if(ext == wxT("exe") || ext == wxT("") || ext == wxT("xpm") || ext == wxT("png")) { continue; }
                    fn.MakeRelativeTo(privateFolder);
                    files.push_back(fn);
                }
            }
        }

        // write the content of the files into the tempfile
        wxString content;
        for(size_t i = 0; i < files.size(); i++) {
            wxFileName fn(files.at(i));
            content << fn.GetFullPath(wxPATH_UNIX) << wxT("\n");
        }
        FileUtils::WriteFileContent(list_file, content, wxConvUTF8);
    }

    return list_file.GetFullPath();
}

void Cscope::DoCscopeCommand(const wxString& command, const wxString& findWhat, const wxString& endMsg)
{
    // We haven't yet found a valid cscope exe, so look for one
    wxString where;
    if(!ExeLocator::Locate(GetCscopeExeName(), where)) {
        wxString msg;
        msg << _("I can't find 'cscope' anywhere. Please check if it's installed.") << wxT('\n')
            << _("Or tell me where it can be found, from the menu: 'Plugins | CScope | Settings'");
        wxMessageBox(msg, _("CScope not found"), wxOK | wxCENTER | wxICON_WARNING);
        return;
    }

    // set the focus to the cscope tab
    Notebook* book = m_mgr->GetOutputPaneNotebook();

    // make sure that the Output pane is visible
    wxAuiManager* aui = m_mgr->GetDockingManager();
    if(aui) {
        wxAuiPaneInfo& info = aui->GetPane(wxT("Output View"));
        if(info.IsOk() && !info.IsShown()) {
            info.Show();
            aui->Update();
        }
    }

    wxString curSel = book->GetPageText((size_t)book->GetSelection());
    if(curSel != CSCOPE_NAME) {
        for(size_t i = 0; i < (size_t)book->GetPageCount(); i++) {
            if(book->GetPageText(i) == CSCOPE_NAME) {
                book->SetSelection(i);
                break;
            }
        }
    }

    // create the search thread and return
    CscopeRequest* req = new CscopeRequest();
    req->SetOwner(this);
    req->SetCmd(command);
    req->SetEndMsg(endMsg);
    req->SetFindWhat(findWhat);
    req->SetWorkingDir(GetWorkingDirectory());

    CScopeThreadST::Get()->Add(req);
}

void Cscope::OnFindSymbol(wxCommandEvent& e)
{
    wxString word = GetSearchPattern();
    if(!word.IsEmpty()) { DoFindSymbol(word); }
}

void Cscope::OnFindGlobalDefinition(wxCommandEvent& e)
{
    wxString word = GetSearchPattern();
    if(word.IsEmpty()) { return; }
    m_cscopeWin->Clear();
    wxString list_file = DoCreateListFile(false);

    // Do the actual search
    wxString command;
    wxString endMsg;
    command << GetCscopeExeName() << wxT(" -d -L -1 ") << word << wxT(" -i ") << list_file;
    endMsg << _("cscope results for: find global definition of '") << word << wxT("'");
    DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnFindFunctionsCalledByThisFunction(wxCommandEvent& e)
{
    wxString word = GetSearchPattern();
    if(word.IsEmpty()) { return; }

    m_cscopeWin->Clear();
    wxString list_file = DoCreateListFile(false);

    // get the rebuild option
    wxString rebuildOption = wxT("");
    CScopeConfData settings;

    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
    if(!settings.GetRebuildOption()) { rebuildOption = wxT(" -d"); }

    // Do the actual search
    wxString command;
    wxString endMsg;
    command << GetCscopeExeName() << rebuildOption << wxT(" -L -2 ") << word << wxT(" -i ") << list_file;
    endMsg << _("cscope results for: functions called by '") << word << wxT("'");
    DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnFindFunctionsCallingThisFunction(wxCommandEvent& e)
{
    wxString word = GetSearchPattern();
    if(word.IsEmpty()) { return; }

    m_cscopeWin->Clear();
    wxString list_file = DoCreateListFile(false);

    // get the rebuild option
    wxString rebuildOption = wxT("");
    CScopeConfData settings;

    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
    if(!settings.GetRebuildOption()) { rebuildOption = wxT(" -d"); }

    // Do the actual search
    wxString command;
    wxString endMsg;
    command << GetCscopeExeName() << rebuildOption << wxT(" -L -3 ") << word << wxT(" -i ") << list_file;
    endMsg << _("cscope results for: functions calling '") << word << wxT("'");
    DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnFindFilesIncludingThisFname(wxCommandEvent& e)
{
    wxString word = m_mgr->GetActiveEditor()->GetSelection();
    if(word.IsEmpty()) {
        // If there's no selection, try for the caret word
        // That'll either be (rubbish, or) a filename
        // or it'll be the 'h'of filename.h
        // Cscope can cope with just a filename
        word = m_mgr->GetActiveEditor()->GetWordAtCaret();
        if(word == wxT("h")) {
            long pos = m_mgr->GetActiveEditor()->GetCurrentPosition();
            long start = m_mgr->GetActiveEditor()->WordStartPos(pos - 2, true);
            wxString name = m_mgr->GetActiveEditor()->GetTextRange(start, pos - 2);
            // Append the .h  Cscope would be happy with just foo,
            // but would also return #include foobar.h which isn't what's been requested
            word = name + wxT(".h");
        }
        if(word.IsEmpty()) { return; }
    }

    m_cscopeWin->Clear();
    wxString list_file = DoCreateListFile(false);

    // get the rebuild option
    wxString rebuildOption = wxT("");
    CScopeConfData settings;

    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
    if(!settings.GetRebuildOption()) { rebuildOption = wxT(" -d"); }

    // Do the actual search
    wxString command;
    wxString endMsg;
    command << GetCscopeExeName() << rebuildOption << wxT(" -L -8 ") << word << wxT(" -i ") << list_file;
    endMsg << _("cscope results for: files that #include '") << word << wxT("'");
    DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnCreateDB(wxCommandEvent& e)
{
    // sanity
    if(!m_mgr->IsWorkspaceOpen() && !clFileSystemWorkspace::Get().IsOpen()) { return; }

    m_cscopeWin->Clear();
    wxString list_file = DoCreateListFile(true);

    // get the reverted index option
    wxString command;
    wxString endMsg;
    CScopeConfData settings;

    command << GetCscopeExeName();

    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
    if(settings.GetBuildRevertedIndexOption()) {
        command << wxT(" -q");
        endMsg << _("Recreated inverted CScope DB");
    } else {
        command << wxT(" -b");
        endMsg << _("Recreated CScope DB");
    }

    // Do the actual create db
    // since the process is always running from the workspace
    // directory, there is no need to specify the full path of the list file

    command << wxT(" -L -i cscope_file.list");
    DoCscopeCommand(command, wxEmptyString, endMsg);
}

void Cscope::OnDoSettings(wxCommandEvent& e)
{
    // atm the only setting to set is the cscope filepath
    // First find the current value, if any
    CScopeConfData settings;
    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
    wxString filepath = settings.GetCscopeExe();

    CScopeSettingsDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        settings.SetCscopeExe(dlg.GetPath());
        m_mgr->GetConfigTool()->WriteObject(wxT("CscopeSettings"), &settings);
    }
}

wxString Cscope::GetCscopeExeName()
{
    CScopeConfData settings;
    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
    return settings.GetCscopeExe();
}

void Cscope::OnCScopeThreadEnded(wxCommandEvent& e)
{
    CScopeResultTable_t* result = (CScopeResultTable_t*)e.GetClientData();
    m_cscopeWin->BuildTable(result);
}

void Cscope::OnCScopeThreadUpdateStatus(wxCommandEvent& e)
{
    CScopeStatusMessage* msg = (CScopeStatusMessage*)e.GetClientData();
    if(msg) {
        m_cscopeWin->SetMessage(msg->GetMessage(), msg->GetPercentage());

        if(msg->GetFindWhat().IsEmpty() == false) { m_cscopeWin->SetFindWhat(msg->GetFindWhat()); }
        delete msg;
    }
    e.Skip();
}

void Cscope::OnCscopeUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    bool isEditor = m_mgr->GetActiveEditor() ? true : false;
    e.Enable((m_mgr->IsWorkspaceOpen() || clFileSystemWorkspace::Get().IsOpen()) && isEditor);
}

void Cscope::OnWorkspaceOpenUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(m_mgr->IsWorkspaceOpen() || clFileSystemWorkspace::Get().IsOpen());
}

void Cscope::OnFindUserInsertedSymbol(wxCommandEvent& WXUNUSED(e))
{
    wxString word = GetSearchPattern();
    if(word.IsEmpty()) return;

    DoFindSymbol(word);
}

wxString Cscope::GetSearchPattern() const
{
    wxString pattern;
    if(m_mgr->IsShutdownInProgress()) { return pattern; }

    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) { pattern = editor->GetWordAtCaret(); }

    if(pattern.IsEmpty()) {
        pattern = wxGetTextFromUser(_("Enter the symbol to search for:"), _("cscope: find symbol"), wxT(""),
                                    m_mgr->GetTheApp()->GetTopWindow());
    }

    return pattern;
}

void Cscope::DoFindSymbol(const wxString& word)
{
    m_cscopeWin->Clear();
    wxString list_file = DoCreateListFile(false);

    // get the rebuild option
    wxString rebuildOption = wxT("");
    CScopeConfData settings;

    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
    if(!settings.GetRebuildOption()) { rebuildOption = wxT(" -d"); }

    // Do the actual search
    wxString command;
    wxString endMsg;
    command << GetCscopeExeName() << rebuildOption << wxT(" -L -0 ") << word << wxT(" -i ") << list_file;
    endMsg << wxT("cscope results for: find C symbol '") << word << wxT("'");
    DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnEditorContentMenu(clContextMenuEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);
    if(FileExtManager::IsCxxFile(editor->GetFileName())) {
        event.GetMenu()->Append(wxID_ANY, _("CScope"), CreateEditorPopMenu());
    }
}

wxString Cscope::GetWorkingDirectory() const
{
    if(!IsWorkspaceOpen()) { return wxEmptyString; }
    
    if(clFileSystemWorkspace::Get().IsOpen()) {
        wxFileName fn = clFileSystemWorkspace::Get().GetFileName();
        fn.AppendDir(".codelite");
        return fn.GetPath();
    } else {
        return clCxxWorkspaceST::Get()->GetPrivateFolder();
    }
}

bool Cscope::IsWorkspaceOpen() const
{
    return clFileSystemWorkspace::Get().IsOpen() || clCxxWorkspaceST::Get()->IsOpen();
}
