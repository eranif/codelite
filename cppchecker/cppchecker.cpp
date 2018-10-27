//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppchecker.cpp
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

#include "Notebook.h"
#include "cl_process.h"
#include "cppchecker.h"
#include "cppcheckreportpage.h"
#include "cppchecksettingsdlg.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "imanager.h"
#include "jobqueue.h"
#include "macros.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "project.h"
#include "workspace.h"
#include <wx/app.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/sstream.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

static CppCheckPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new CppCheckPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah & Jérémie (jfouche)"));
    info.SetName(wxT("CppChecker"));
    info.SetDescription(_("CppChecker integration for CodeLite IDE"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CppCheckPlugin::CppCheckPlugin(IManager* manager)
    : IPlugin(manager)
    , m_cppcheckProcess(NULL)
    , m_canRestart(true)
    , m_explorerSepItem(NULL)
    , m_workspaceSepItem(NULL)
    , m_projectSepItem(NULL)
    , m_view(NULL)
    , m_analysisInProgress(false)
    , m_fileCount(0)
    , m_fileProcessed(1)
{
    FileExtManager::Init();

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &CppCheckPlugin::OnCppCheckReadData, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CppCheckPlugin::OnCppCheckTerminated, this);

    m_longName = _("CppCheck integration for CodeLite IDE");
    m_shortName = wxT("CppCheck");

    // Load settings
    m_mgr->GetConfigTool()->ReadObject("CppCheck", &m_settings);
    // Now set default suppressions if none have been serialised
    m_settings.SetDefaultSuppressedWarnings();
    // NB we can't load any project-specific settings here, as the workspace won't yet have loaded. We do it just before
    // they're used

    // Connect events
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_settings_item"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CppCheckPlugin::OnSettingsItem), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_settings_item_project"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CppCheckPlugin::OnSettingsItemProject), NULL,
                                (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_editor_item"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CppCheckPlugin::OnCheckFileEditorItem), NULL,
                                (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL,
                                (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_workspace_item"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_project_item"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem), NULL, (wxEvtHandler*)this);

    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),
                                  NULL, this);

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &CppCheckPlugin::OnEditorContextMenu, this);
    m_view = new CppCheckReportPage(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

    //	wxBookCtrlBase *book = m_mgr->GetOutputPaneNotebook();
    m_mgr->GetOutputPaneNotebook()->AddPage(m_view, _("CppCheck"), false,
                                            m_mgr->GetStdIcons()->LoadBitmap("check-all"));
    m_tabHelper.reset(new clTabTogglerHelper(_("CppCheck"), m_view, "", NULL));
    m_tabHelper->SetOutputTabBmp(m_mgr->GetStdIcons()->LoadBitmap("check-all"));
}

CppCheckPlugin::~CppCheckPlugin() {}

void CppCheckPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void CppCheckPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item =
        new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, wxT("CppCheck"), menu);
}

void CppCheckPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileExplorer) {
        if(!menu->FindItem(XRCID("CPPCHECK_EXPLORER_POPUP"))) {
            m_explorerSepItem = menu->PrependSeparator();
            menu->Prepend(XRCID("CPPCHECK_EXPLORER_POPUP"), _("CppCheck"), CreateFileExplorerPopMenu());
        }

    } else if(type == MenuTypeFileView_Workspace) {
        if(!menu->FindItem(XRCID("CPPCHECK_WORKSPACE_POPUP"))) {
            m_workspaceSepItem = menu->PrependSeparator();
            menu->Prepend(XRCID("CPPCHECK_WORKSPACE_POPUP"), _("CppCheck"), CreateWorkspacePopMenu());
        }

    } else if(type == MenuTypeFileView_Project) {
        if(!menu->FindItem(XRCID("CPPCHECK_PROJECT_POPUP"))) {
            m_projectSepItem = menu->PrependSeparator();
            menu->Prepend(XRCID("CPPCHECK_PROJECT_POPUP"), _("CppCheck"), CreateProjectPopMenu());
        }
    }
}

void CppCheckPlugin::UnPlug()
{
    m_tabHelper.reset(NULL);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &CppCheckPlugin::OnCppCheckReadData, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &CppCheckPlugin::OnCppCheckTerminated, this);

    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_settings_item"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CppCheckPlugin::OnSettingsItem), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_settings_item_project"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CppCheckPlugin::OnSettingsItemProject), NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_editor_item"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CppCheckPlugin::OnCheckFileEditorItem), NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_workspace_item"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem), NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_project_item"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem), NULL,
                                   (wxEvtHandler*)this);

    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &CppCheckPlugin::OnEditorContextMenu, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),
                                     NULL, this);

    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            break;
        }
    }
    m_view->Destroy();

    // terminate the cppcheck daemon
    wxDELETE(m_cppcheckProcess);
}

wxMenu* CppCheckPlugin::CreateFileExplorerPopMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interested in is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cppcheck_fileexplorer_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

wxMenu* CppCheckPlugin::CreateEditorPopMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interested in is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cppcheck_editor_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

wxMenu* CppCheckPlugin::CreateProjectPopMenu()
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cppcheck_project_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cppcheck_settings_item_project"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

wxMenu* CppCheckPlugin::CreateWorkspacePopMenu()
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cppcheck_workspace_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

void CppCheckPlugin::OnCheckFileEditorItem(wxCommandEvent& e)
{
    if(m_cppcheckProcess) {
        clLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
        return;
    }

    ProjectPtr proj;
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) {
        wxString projectName = editor->GetProjectName();
        if(!projectName.IsEmpty()) { proj = clCxxWorkspaceST::Get()->GetProject(projectName); }
        m_filelist.Add(editor->GetFileName().GetFullPath());
    }

    DoStartTest();
}

void CppCheckPlugin::OnCheckFileExplorerItem(wxCommandEvent& e)
{
    if(m_cppcheckProcess) {
        clLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
        return;
    }

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
    for(size_t i = 0; i < item.m_paths.GetCount(); ++i) {
        if(wxDir::Exists(item.m_paths.Item(i))) {
            // directory
            GetFileListFromDir(item.m_paths.Item(i));
        } else {
            // filename
            m_filelist.Add(item.m_paths.Item(i));
        }
    }
    DoStartTest();
}

void CppCheckPlugin::OnCheckWorkspaceItem(wxCommandEvent& e)
{
    if(m_cppcheckProcess) {
        clLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
        return;
    }

    if(!m_mgr->GetWorkspace() || !m_mgr->IsWorkspaceOpen()) { return; }

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_itemType == ProjectItem::TypeWorkspace) {

        // retrieve complete list of source files of the workspace
        wxArrayString projects;
        wxString err_msg;
        std::vector<wxFileName> tmpfiles;
        m_mgr->GetWorkspace()->GetProjectList(projects);

        for(size_t i = 0; i < projects.GetCount(); i++) {
            ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
            if(proj) { proj->GetFilesAsVectorOfFileName(tmpfiles); }
        }

        // only C/C++ files
        for(size_t i = 0; i < tmpfiles.size(); i++) {
            if(FileExtManager::GetType(tmpfiles.at(i).GetFullPath()) == FileExtManager::TypeSourceC ||
               FileExtManager::GetType(tmpfiles.at(i).GetFullPath()) == FileExtManager::TypeSourceCpp) {
                m_filelist.Add(tmpfiles.at(i).GetFullPath());
            }
        }
    }
    DoStartTest();
}

ProjectPtr CppCheckPlugin::FindSelectedProject()
{
    ProjectPtr proj = NULL;

    if(!m_mgr->GetWorkspace() || !m_mgr->IsWorkspaceOpen()) { return proj; }

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_itemType == ProjectItem::TypeProject) {
        wxString project_name(item.m_text);
        wxString err_msg;
        proj = m_mgr->GetWorkspace()->FindProjectByName(project_name, err_msg);
    }

    return proj;
}

void CppCheckPlugin::OnCheckProjectItem(wxCommandEvent& e)
{
    if(m_cppcheckProcess) {
        clLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
        return;
    }

    ProjectPtr proj = FindSelectedProject();
    if(!proj) { return; }

    // retrieve complete list of source files of the workspace
    std::vector<wxFileName> tmpfiles;
    proj->GetFilesAsVectorOfFileName(tmpfiles);

    // only C/C++ files
    for(size_t i = 0; i < tmpfiles.size(); i++) {
        if(FileExtManager::GetType(tmpfiles.at(i).GetFullPath()) == FileExtManager::TypeSourceC ||
           FileExtManager::GetType(tmpfiles.at(i).GetFullPath()) == FileExtManager::TypeSourceCpp) {
            m_filelist.Add(tmpfiles.at(i).GetFullPath());
        }
    }

    DoStartTest(proj);
}

void CppCheckPlugin::OnCppCheckTerminated(clProcessEvent& e)
{
    m_filelist.Clear();
    wxDELETE(m_cppcheckProcess);

    m_view->PrintStatusMessage();
    m_view->GotoFirstError();
}

void CppCheckPlugin::OnSettingsItem(wxCommandEvent& WXUNUSED(e)) { DoSettingsItem(); }

void CppCheckPlugin::OnSettingsItemProject(wxCommandEvent& WXUNUSED(e))
{
    ProjectPtr proj = FindSelectedProject();
    DoSettingsItem(proj);
}

void CppCheckPlugin::DoSettingsItem(ProjectPtr project /*= NULL*/)
{
    // Find the default path for the CppCheckSettingsDialog's wxFileDialog
    wxString defaultpath;
    IEditor* ed = m_mgr->GetActiveEditor();
    if(ed && ed->GetFileName().IsOk()) { defaultpath = ed->GetFileName().GetPath(); }

    // If there's an active project, first load any project-specific settings: definitions and undefines
    // (We couldn't do that with the rest of the settings as the workspace hadn't yet been loaded)
    m_settings.LoadProjectSpecificSettings(
        project); // NB we still do this if !project, as that will clear any stale settings

    CppCheckSettingsDialog dlg(m_mgr->GetTheApp()->GetTopWindow(), &m_settings, m_mgr->GetConfigTool(), defaultpath,
                               project.Get() != NULL);
    if(dlg.ShowModal() == wxID_OK) {
        m_mgr->GetConfigTool()->WriteObject(wxT("CppCheck"), &m_settings);
        if(project) {
            // Also save any project-specific settings: definitions and undefines
            wxString definitions = wxJoin(m_settings.GetDefinitions(), ',');
            wxString undefines = wxJoin(m_settings.GetUndefines(), ',');
            if(!(definitions.empty() && undefines.empty())) {
                project->SetPluginData("CppCheck", definitions + ';' + undefines);
            } else {
                project->SetPluginData("CppCheck", "");
            }
        }
    }
}

void CppCheckPlugin::GetFileListFromDir(const wxString& root)
{
    m_filelist.Clear();
    wxArrayString tmparr;
    wxDir::GetAllFiles(root, &tmparr);

    for(size_t i = 0; i < tmparr.GetCount(); i++) {
        switch(FileExtManager::GetType(tmparr.Item(i))) {
        case FileExtManager::TypeSourceC:
        case FileExtManager::TypeSourceCpp: {
            m_filelist.Add(tmparr.Item(i));
            break;
        }

        default:
            break;
        }
    }
}

void CppCheckPlugin::DoProcess(ProjectPtr proj)
{
    wxString command = DoGetCommand(proj);
    m_view->AppendLine(wxString::Format(_("Starting cppcheck: %s\n"), command.c_str()));

#if defined(__WXMSW__)
    // Under Windows, we set the working directory to the binary folder
    // so the configurtion files can be found
    CL_DEBUG("CppCheck: Working directory: %s", clStandardPaths::Get().GetBinFolder());
    CL_DEBUG("CppCheck: Command: %s", command);
    m_cppcheckProcess = CreateAsyncProcess(this, command, IProcessCreateDefault, clStandardPaths::Get().GetBinFolder());
#elif defined(__WXOSX__)
    CL_DEBUG("CppCheck: Working directory: %s", clStandardPaths::Get().GetDataDir());
    CL_DEBUG("CppCheck: Command: %s", command);
    m_cppcheckProcess = CreateAsyncProcess(this, command, IProcessCreateDefault, clStandardPaths::Get().GetDataDir());

#else
    m_cppcheckProcess = CreateAsyncProcess(this, command);
#endif
    if(!m_cppcheckProcess) {
        wxMessageBox(_("Failed to launch codelite_cppcheck process!"), _("Warning"), wxOK | wxCENTER | wxICON_WARNING);
        return;
    }
}

/**
 * Ensure that the CppCheck tab is visible
 */
void CppCheckPlugin::SetTabVisible(bool clearContent)
{
    // Make sure that the Output pane is visible
    wxAuiManager* aui = m_mgr->GetDockingManager();
    if(aui) {
        wxAuiPaneInfo& info = aui->GetPane(wxT("Output View"));
        if(info.IsOk() && !info.IsShown()) {
            info.Show();
            aui->Update();
        }
    }

    // Set the focus to the CppCheck tab
    Notebook* book = m_mgr->GetOutputPaneNotebook();
    if(book->GetPageText((size_t)book->GetSelection()) != wxT("CppCheck")) {
        for(size_t i = 0; i < book->GetPageCount(); i++) {
            if(book->GetPageText(i) == wxT("CppCheck")) {
                book->SetSelection(i);
                break;
            }
        }
    }

    // clear the view contents
    if(clearContent) {
        m_view->Clear();
        m_fileCount = m_filelist.GetCount();
        m_fileProcessed = 1;
    }
}

void CppCheckPlugin::StopAnalysis()
{
    // Clear the files queue
    if(m_cppcheckProcess) {
        // terminate the m_cppcheckProcess
        m_cppcheckProcess->Terminate();
    }
}

size_t CppCheckPlugin::GetProgress()
{
    double progress = (((double)m_fileProcessed) / (double)m_fileCount) * 100;
    return (size_t)progress;
}

void CppCheckPlugin::RemoveExcludedFiles()
{
    wxArrayString exclude = m_settings.GetExcludeFiles();

    wxArrayString tmpfiles(m_filelist);
    m_filelist.Clear();

    for(size_t i = 0; i < tmpfiles.GetCount(); i++) {
        wxFileName fn(tmpfiles.Item(i));
        if(exclude.Index(fn.GetFullPath()) == wxNOT_FOUND) {
            // file does not exist in the excluded files list
            // add it
            m_filelist.Add(tmpfiles.Item(i));
        }
    }
}

void CppCheckPlugin::OnWorkspaceClosed(wxCommandEvent& e)
{
    m_view->Clear();
    e.Skip();
}

void CppCheckPlugin::DoStartTest(ProjectPtr proj /*=NULL*/)
{
    RemoveExcludedFiles();
    if(!m_filelist.GetCount()) {
        wxMessageBox(_("No files to check"), "CppCheck", wxOK | wxCENTRE, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }
    SetTabVisible(true);
    m_view->Clear();
    m_view->SetGaugeRange(m_filelist.GetCount());

    // We need to load any project-specific settings: definitions and undefines
    // (We couldn't do that with the rest of the settings as the workspace hadn't yet been loaded)
    m_settings.LoadProjectSpecificSettings(proj); // NB we still do this if !proj, as that will clear any stale settings

    // Start the test
    DoProcess(proj);
}

wxString CppCheckPlugin::DoGetCommand(ProjectPtr proj)
{
    // Linux / Mac way: spawn the process and execute the command
    wxString cmd, path;
    path = clStandardPaths::Get().GetBinaryFullPath("codelite_cppcheck");
    ::WrapWithQuotes(path);

    wxString fileList = DoGenerateFileList();
    if(fileList.IsEmpty()) return wxT("");

    // build the command
    cmd << path << " ";
    cmd << m_settings.GetOptions();

    // Append here project specifc search paths
    if(proj) {
        wxArrayString projectSearchPaths = proj->GetIncludePaths();
        for(size_t i = 0; i < projectSearchPaths.GetCount(); ++i) {
            wxFileName fnIncPath(projectSearchPaths.Item(i), "");
            wxString includePath = fnIncPath.GetPath();
            ::WrapWithQuotes(includePath);
            cmd << " -I" << includePath;
        }

        wxArrayString projMacros = proj->GetPreProcessors();
        for(size_t i = 0; i < projMacros.GetCount(); ++i) {
            cmd << " -D" << projMacros.Item(i);
        }
    }

    cmd << wxT(" --file-list=");
    ::WrapWithQuotes(fileList);
    cmd << fileList << " ";
    CL_DEBUG("cppcheck command: %s", cmd);
    ::WrapInShell(cmd);
    return cmd;
}

wxString CppCheckPlugin::DoGenerateFileList()
{
    // create temporary file and save the file there
    wxFileName fnFileList(clCxxWorkspaceST::Get()->GetPrivateFolder(), "cppcheck.list");

    // create temporary file and save the file there
    wxFFile file(fnFileList.GetFullPath(), wxT("w+b"));
    if(!file.IsOpened()) {
        wxMessageBox(_("Failed to open temporary file ") + fnFileList.GetFullPath(), _("Warning"),
                     wxOK | wxCENTER | wxICON_WARNING);
        return wxEmptyString;
    }

    wxString content;
    for(size_t i = 0; i < m_filelist.GetCount(); i++) {
        content << m_filelist.Item(i) << wxT("\n");
    }

    file.Write(content);
    file.Flush();
    file.Close();
    return fnFileList.GetFullPath();
}

void CppCheckPlugin::OnCppCheckReadData(clProcessEvent& e)
{
    e.Skip();
    m_view->AppendLine(e.GetOutput());
}

void CppCheckPlugin::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(FileExtManager::IsCxxFile(editor->GetFileName())) {
        event.GetMenu()->Append(wxID_ANY, _("CppCheck"), CreateEditorPopMenu());
    }
}
