/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#include <wx/busyinfo.h>
#include <wx/filedlg.h>

#include "async_executable_cmd.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "workspace.h"

#include "asyncprocess.h"
#include "memcheck.h"
#include "memcheckdefs.h"
#include "memcheckoutputview.h"
#include "memchecksettings.h"
#include "memchecksettingsdialog.h"
#include "memcheckui.h"
#include "processreaderthread.h"
#include "valgrindprocessor.h"
#include "build_config.h"
#include "macromanager.h"
#include "globals.h"

static MemCheckPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new MemCheckPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("pavel.iqx"));
    info.SetName(wxT("MemCheck"));
    info.SetDescription(_("MemCheck plugin detects memory leaks. Uses Valgrind (memcheck tool) as backend."));
    info.SetVersion(wxT("0.5"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

MemCheckPlugin::MemCheckPlugin(IManager* manager)
    : IPlugin(manager)
    , m_memcheckProcessor(NULL)
{
    m_terminal.Bind(wxEVT_TERMINAL_COMMAND_EXIT, &MemCheckPlugin::OnProcessTerminated, this);
    m_terminal.Bind(wxEVT_TERMINAL_COMMAND_OUTPUT, &MemCheckPlugin::OnProcessOutput, this);

    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckPlugin constructor"));
    m_longName = _("Detects memory management problems. Uses Valgrind - memcheck skin.");
    m_shortName = wxT("MemCheck");

    // menu File and OutputView controls
    m_mgr->GetTheApp()->Connect(MemCheckOutputViewBase::ID_TOOL_STOP_PROCESS, wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(MemCheckPlugin::OnStopProcess), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Connect(MemCheckOutputViewBase::ID_TOOL_STOP_PROCESS, wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(MemCheckPlugin::OnStopProcessUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Connect(XRCID("memcheck_check_active_project"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(MemCheckPlugin::OnCheckAtiveProject), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("memcheck_check_active_project"), wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Connect(XRCID("memcheck_check_popup_project"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(MemCheckPlugin::OnCheckPopupProject), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("memcheck_check_popup_project"), wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Connect(XRCID("memcheck_check_popup_editor"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(MemCheckPlugin::OnCheckPopupEditor), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("memcheck_check_popup_editor"), wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Connect(XRCID("memcheck_import"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(MemCheckPlugin::OnImportLog), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("memcheck_import"), wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Connect(XRCID("memcheck_settings"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(MemCheckPlugin::OnSettings), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("memcheck_settings"), wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    // EventNotifier::Get()->Connect( wxEVT_INIT_DONE, wxCommandEventHandler(MemCheckPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(MemCheckPlugin::OnWorkspaceLoaded),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(MemCheckPlugin::OnWorkspaceClosed),
                                  NULL, this);

    // CL_DEBUG1(PLUGIN_PREFIX("adding 'Output View' notebook pane"));
    m_outputView = new MemCheckOutputView(m_mgr->GetOutputPaneNotebook(), this, m_mgr);
    m_mgr->GetOutputPaneNotebook()->AddPage(m_outputView, _("MemCheck"), false,
                                            wxXmlResource::Get()->LoadBitmap(wxT("check")));
    m_tabHelper.reset(new clTabTogglerHelper(_("MemCheck"), m_outputView, "", NULL));
    m_tabHelper->SetOutputTabBmp(wxXmlResource::Get()->LoadBitmap(wxT("check")));

    m_settings = new MemCheckSettings();
    GetSettings()->LoadFromConfig();
    GetSettings()->SavaToConfig();

    // sets defaults also in OutputView page - important to be here
    ApplySettings();
}

MemCheckPlugin::~MemCheckPlugin()
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckPlugin destroyed"));
    wxDELETE(m_memcheckProcessor);
    wxDELETE(m_settings);
}

void MemCheckPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void MemCheckPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckPlugin::CreatePluginMenu()"));
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("memcheck_check_active_project"), wxT("&Run MemCheck"), wxEmptyString,
                          wxITEM_NORMAL);
    item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_check")));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("memcheck_import"), wxT("&Load MemCheck log from file..."), wxEmptyString,
                          wxITEM_NORMAL);
    item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_import")));
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("memcheck_settings"), wxT("&Settings..."), wxEmptyString, wxITEM_NORMAL);
    item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_settings")));
    menu->Append(item);

    item = new wxMenuItem(pluginsMenu, wxID_ANY, wxT("MemCheck"), wxEmptyString, wxITEM_NORMAL, menu);
    item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_check")));
    pluginsMenu->Append(item);
}

void MemCheckPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Project) {
        // items for the file view/Project context menu
        if(!menu->FindItem(XRCID("memcheck_MenuTypeFileView_Project"))) {
            wxMenu* subMenu = new wxMenu();
            wxMenuItem* item(NULL);

            item = new wxMenuItem(subMenu, XRCID("memcheck_check_popup_project"), wxT("&Run MemCheck"), wxEmptyString,
                                  wxITEM_NORMAL);
            item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_check")));
            subMenu->Append(item);

            item = new wxMenuItem(subMenu, XRCID("memcheck_import"), wxT("&Load MemCheck log from file..."),
                                  wxEmptyString, wxITEM_NORMAL);
            item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_import")));
            subMenu->Append(item);

            subMenu->AppendSeparator();

            item =
                new wxMenuItem(subMenu, XRCID("memcheck_settings"), wxT("&Settings..."), wxEmptyString, wxITEM_NORMAL);
            item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_settings")));
            subMenu->Append(item);

            menu->PrependSeparator();
            item = new wxMenuItem(menu, XRCID("memcheck_MenuTypeFileView_Project"), wxT("MemCheck"), wxEmptyString,
                                  wxITEM_NORMAL, subMenu);
            item->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("memcheck_check")));
            menu->Prepend(item);
        }
    }
}

void MemCheckPlugin::UnPlug()
{
    m_tabHelper.reset(NULL);
    m_terminal.Unbind(wxEVT_TERMINAL_COMMAND_EXIT, &MemCheckPlugin::OnProcessTerminated, this);
    m_terminal.Unbind(wxEVT_TERMINAL_COMMAND_OUTPUT, &MemCheckPlugin::OnProcessOutput, this);

    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_check_active_project"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(MemCheckPlugin::OnCheckAtiveProject), NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_check_active_project"), wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_check_popup_project"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(MemCheckPlugin::OnCheckPopupProject), NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_check_popup_project"), wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_check_popup_editor"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(MemCheckPlugin::OnCheckPopupEditor), NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_check_popup_editor"), wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_import"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(MemCheckPlugin::OnImportLog), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_import"), wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_settings"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(MemCheckPlugin::OnSettings), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("memcheck_settings"), wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(MemCheckPlugin::OnMemCheckUI), NULL, (wxEvtHandler*)this);

    // EventNotifier::Get()->Disconnect( wxEVT_INIT_DONE, wxCommandEventHandler(MemCheckPlugin::OnInitDone), NULL,
    // this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(MemCheckPlugin::OnWorkspaceLoaded),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(MemCheckPlugin::OnWorkspaceClosed),
                                     NULL, this);

    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_outputView == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_outputView->Destroy();
            break;
        }
    }
}

void MemCheckPlugin::OnWorkspaceLoaded(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckPlugin::OnWorkspaceLoaded()"));
    ApplySettings();
    event.Skip();
}

void MemCheckPlugin::OnWorkspaceClosed(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckPlugin::OnWorkspaceClosed()"));
    ApplySettings();
    event.Skip();
}

bool MemCheckPlugin::IsReady(wxUpdateUIEvent& event)
{
    bool ready = !m_mgr->IsBuildInProgress() && !m_terminal.IsRunning();
    int id = event.GetId();
    if(id == XRCID("memcheck_check_active_project")) {
        ready &= !m_mgr->GetWorkspace()->GetActiveProjectName().IsEmpty();
    }
    return ready;
}

void MemCheckPlugin::ApplySettings(bool loadLastErrors)
{
    wxDELETE(m_memcheckProcessor);
    m_memcheckProcessor = new ValgrindMemcheckProcessor(GetSettings());
    if(loadLastErrors) {
        m_outputView->LoadErrors();

    } else {
        m_outputView->Clear();
    }
}

void MemCheckPlugin::SwitchToMyPage()
{
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_outputView == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->ChangeSelection(i);
            break;
        }
    }
}

void MemCheckPlugin::OnCheckAtiveProject(wxCommandEvent& event)
{
    CHECK_CL_SHUTDOWN()
    clCxxWorkspace* workspace = m_mgr->GetWorkspace();
    if(workspace) { CheckProject(workspace->GetActiveProjectName()); }
}

void MemCheckPlugin::OnCheckPopupProject(wxCommandEvent& event)
{
    CHECK_CL_SHUTDOWN()
    ProjectPtr project = m_mgr->GetSelectedProject();
    if(project) { CheckProject(project->GetName()); }
}

void MemCheckPlugin::OnCheckPopupEditor(wxCommandEvent& event)
{
    CHECK_CL_SHUTDOWN()
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) { CheckProject(editor->GetProjectName()); }
}

void MemCheckPlugin::CheckProject(const wxString& projectName)
{
    if(m_terminal.IsRunning()) {
        ::wxMessageBox(_("Another instance is already running. Please stop it before executing another one"),
                       "CodeLite", wxICON_WARNING | wxCENTER | wxOK);
        return;
    }

    wxString errMsg;
    ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
    wxString path = project->GetFileName().GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    wxString wd;
    wxString command = PrepareCommand(projectName, wd);

    m_outputView->Clear(); // To reduce the risk of confusion, clear any current errors before running
    DirSaver ds;
    EnvSetter envGuard(m_mgr->GetEnv());
    wxSetWorkingDirectory(path);
    wxSetWorkingDirectory(wd);
    m_mgr->AppendOutputTabText(kOutputTab_Output, _("Launching MemCheck...\n"));
    m_mgr->AppendOutputTabText(kOutputTab_Output, wxString()
                                                      << _("Working directory is set to: ") << ::wxGetCwd() << "\n");
    wxString cmd;
    wxString cmdArgs;
    m_memcheckProcessor->GetExecutionCommand(command, cmd, cmdArgs);
    m_mgr->AppendOutputTabText(kOutputTab_Output, wxString()
                                                      << "MemCheck command: " << command << " " << cmdArgs << "\n");
    m_terminal.ExecuteConsole(cmd, true, cmdArgs, "", wxString::Format("MemCheck: %s", projectName));
}

void MemCheckPlugin::OnImportLog(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckPlugin::OnImportLog()"));

    wxFileDialog openFileDialog(m_mgr->GetTheApp()->GetTopWindow(), wxT("Open log file"), "", "",
                                "xml files (*.xml)|*.xml|all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(openFileDialog.ShowModal() == wxID_CANCEL) return;

    wxWindowDisabler disableAll;
    wxBusyInfo wait(wxT(BUSY_MESSAGE));
    m_mgr->GetTheApp()->Yield();

    if(!m_memcheckProcessor->Process(openFileDialog.GetPath()))
        wxMessageBox(wxT("Output log file cannot be properly loaded."), wxT("Processing error."), wxICON_ERROR);

    m_outputView->LoadErrors();
    SwitchToMyPage();
}

void MemCheckPlugin::OnSettings(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckPlugin::OnSettings()"));
    MemCheckSettingsDialog dialog(m_mgr->GetTheApp()->GetTopWindow(), m_settings);
    if(dialog.ShowModal() == wxID_OK) ApplySettings();
}

void MemCheckPlugin::OnMemCheckUI(wxUpdateUIEvent& event)
{
    CHECK_CL_SHUTDOWN()
    event.Enable(IsReady(event));
}

void MemCheckPlugin::StopProcess()
{
    if(m_terminal.IsRunning()) m_terminal.Terminate();
}

void MemCheckPlugin::OnProcessOutput(clCommandEvent& event)
{
    m_mgr->AppendOutputTabText(kOutputTab_Output, event.GetString());
}

void MemCheckPlugin::OnProcessTerminated(clCommandEvent& event)
{
    m_mgr->AppendOutputTabText(kOutputTab_Output, _("\n-- MemCheck process completed\n"));
    wxBusyInfo wait(wxT(BUSY_MESSAGE));
    m_mgr->GetTheApp()->Yield();

    m_memcheckProcessor->Process();
    m_outputView->LoadErrors();
    SwitchToMyPage();
}

void MemCheckPlugin::OnStopProcess(wxCommandEvent& event)
{
    wxUnusedVar(event);
    StopProcess();
}

void MemCheckPlugin::OnStopProcessUI(wxUpdateUIEvent& event) { event.Enable(IsRunning()); }

wxString MemCheckPlugin::PrepareCommand(const wxString& projectName, wxString& wd)
{
    wd.clear();
    if(!clCxxWorkspaceST::Get()->IsOpen()) { return ""; }

    ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(projectName);
    if(!proj) {
        clWARNING() << "MemCheckPlugin::PrepareCommand(): could not find project:" << projectName;
        return wxEmptyString;
    }

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
    if(!bldConf) {
        clWARNING() << "MemCheckPlugin::PrepareCommand(): failed to find project configuration for project:"
                    << projectName;
        return wxEmptyString;
    }

    wxString projectPath = proj->GetFileName().GetPath();

    // expand variables
    wxString cmd = bldConf->GetCommand();
    cmd = MacroManager::Instance()->Expand(cmd, NULL, projectName);

    wxString cmdArgs = bldConf->GetCommandArguments();
    cmdArgs = MacroManager::Instance()->Expand(cmdArgs, NULL, projectName);

    // Execute command & cmdArgs
    wd = bldConf->GetWorkingDirectory();
    wd = MacroManager::Instance()->Expand(wd, NULL, projectName);

    wxFileName workingDir(wd, "");
    if(workingDir.IsRelative()) { workingDir.MakeAbsolute(projectPath); }

    wxFileName fileExe(cmd);
    if(fileExe.IsRelative()) { fileExe.MakeAbsolute(workingDir.GetPath()); }
    fileExe.Normalize();

#ifdef __WXMSW__
    if(!fileExe.Exists() && fileExe.GetExt().IsEmpty()) {
        // Try with .exe
        wxFileName withExe(fileExe);
        withExe.SetExt("exe");
        if(withExe.Exists()) { fileExe = withExe; }
    }
#endif
    wd = workingDir.GetPath();
    cmd = fileExe.GetFullPath();

    cmd = ::WrapWithQuotes(cmd);
    cmd << " " << cmdArgs;
    clDEBUG() << "Command to execute:" << cmd;
    clDEBUG() << "Working directory:" << wd;
    return cmd;
}
