//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBPlugin.cpp
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

#include "LLDBPlugin.h"

#include "DAPMainView.h"
#include "StringUtils.h"
#include "bookmark_manager.h"
#include "clcommandlineparser.h"
#include "console_frame.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "macromanager.h"

#include <wx/aui/framemanager.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/platinfo.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

static DebugAdapterClient* thePlugin = NULL;
namespace
{

const wxString DAP_MAIN_VIEW = _("Thread, stacks & variables");
const wxString DEBUGGER_NAME = "DAP Debugger";

// Reusing gdb ids so global debugger menu and accelerators work.
const int lldbRunToCursorContextMenuId = XRCID("dbg_run_to_cursor");
const int lldbJumpToCursorContextMenuId = XRCID("dbg_jump_cursor");
const int lldbAddWatchContextMenuId = XRCID("lldb_add_watch");

wxString GetWatchWord(IEditor& editor)
{
    auto word = editor.GetSelection();
    if(word.IsEmpty()) {
        word = editor.GetWordAtCaret();
    }

    // Remove leading and trailing whitespace.
    word.Trim(true);
    word.Trim(false);

    return word;
}

} // namespace

#define CHECK_IS_DAP_CONNECTED()  \
    if(!m_client.IsConnected()) { \
        event.Skip();             \
        return;                   \
    }

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new DebugAdapterClient(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("DebugAdapterClient"));
    info.SetDescription(_("Debug Adapter Client"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

DebugAdapterClient::DebugAdapterClient(IManager* manager)
    : IPlugin(manager)
    , m_isPerspectiveLoaded(false)
{
    m_longName = _("Debug Adapter Client");
    m_shortName = wxT("DebugAdapterClient");

    // UI events
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &DebugAdapterClient::OnIsDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &DebugAdapterClient::OnDebugStart, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &DebugAdapterClient::OnDebugContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT, &DebugAdapterClient::OnDebugNext, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_IN, &DebugAdapterClient::OnDebugStepIn, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_OUT, &DebugAdapterClient::OnDebugStepOut, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &DebugAdapterClient::OnDebugStop, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &DebugAdapterClient::OnDebugIsRunning, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_CAN_INTERACT, &DebugAdapterClient::OnDebugCanInteract, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &DebugAdapterClient::OnToggleBreakpoint, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_INTERRUPT, &DebugAdapterClient::OnToggleInterrupt, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &DebugAdapterClient::OnBuildStarting, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &DebugAdapterClient::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_EXPR_TOOLTIP, &DebugAdapterClient::OnDebugTooltip, this);
    EventNotifier::Get()->Bind(wxEVT_QUICK_DEBUG, &DebugAdapterClient::OnDebugQuickDebug, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CORE_FILE, &DebugAdapterClient::OnDebugCoreFile, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS, &DebugAdapterClient::OnDebugDeleteAllBreakpoints,
                               this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_ATTACH_TO_PROCESS, &DebugAdapterClient::OnDebugAttachToProcess, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS, &DebugAdapterClient::OnDebugEnableAllBreakpoints,
                               this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS, &DebugAdapterClient::OnDebugDisableAllBreakpoints,
                               this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT_INST, &DebugAdapterClient::OnDebugNextInst, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_I, &DebugAdapterClient::OnDebugVOID, this); // not supported

    EventNotifier::Get()->Bind(wxEVT_DBG_UI_SHOW_CURSOR, &DebugAdapterClient::OnDebugShowCursor, this);
    wxTheApp->Bind(wxEVT_MENU, &DebugAdapterClient::OnSettings, this, XRCID("lldb_settings"));

    EventNotifier::Get()->Bind(wxEVT_COMMAND_MENU_SELECTED, &DebugAdapterClient::OnRunToCursor, this,
                               lldbRunToCursorContextMenuId);
    EventNotifier::Get()->Bind(wxEVT_COMMAND_MENU_SELECTED, &DebugAdapterClient::OnJumpToCursor, this,
                               lldbJumpToCursorContextMenuId);

    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &DebugAdapterClient::OnAddWatch, this, lldbAddWatchContextMenuId);

    dap::Initialize(); // register all dap objects

    m_client.Bind(wxEVT_DAP_EXITED_EVENT, &DebugAdapterClient::OnDapExited, this);
    m_client.Bind(wxEVT_DAP_TERMINATED_EVENT, &DebugAdapterClient::OnDapExited, this);
    m_client.Bind(wxEVT_DAP_INITIALIZED_EVENT, &DebugAdapterClient::OnInitializedEvent, this);
    m_client.Bind(wxEVT_DAP_LAUNCH_RESPONSE, &DebugAdapterClient::OnLaunchResponse, this);
    m_client.Bind(wxEVT_DAP_STOPPED_EVENT, &DebugAdapterClient::OnStoppedEvent, this);
    m_client.Bind(wxEVT_DAP_THREADS_RESPONSE, &DebugAdapterClient::OnThreadsResponse, this);
}

void DebugAdapterClient::UnPlug()
{
    DestroyUI();

    // UI events
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &DebugAdapterClient::OnIsDebugger, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &DebugAdapterClient::OnDebugStart, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CONTINUE, &DebugAdapterClient::OnDebugContinue, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT, &DebugAdapterClient::OnDebugNext, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STOP, &DebugAdapterClient::OnDebugStop, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_RUNNING, &DebugAdapterClient::OnDebugIsRunning, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_CAN_INTERACT, &DebugAdapterClient::OnDebugCanInteract, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_IN, &DebugAdapterClient::OnDebugStepIn, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_OUT, &DebugAdapterClient::OnDebugStepOut, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &DebugAdapterClient::OnToggleBreakpoint, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_INTERRUPT, &DebugAdapterClient::OnToggleInterrupt, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &DebugAdapterClient::OnBuildStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &DebugAdapterClient::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_EXPR_TOOLTIP, &DebugAdapterClient::OnDebugTooltip, this);
    EventNotifier::Get()->Unbind(wxEVT_QUICK_DEBUG, &DebugAdapterClient::OnDebugQuickDebug, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CORE_FILE, &DebugAdapterClient::OnDebugCoreFile, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS, &DebugAdapterClient::OnDebugDeleteAllBreakpoints,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_ATTACH_TO_PROCESS, &DebugAdapterClient::OnDebugAttachToProcess, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS, &DebugAdapterClient::OnDebugEnableAllBreakpoints,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS,
                                 &DebugAdapterClient::OnDebugDisableAllBreakpoints, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_I, &DebugAdapterClient::OnDebugVOID, this); // Not supported
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT_INST, &DebugAdapterClient::OnDebugNextInst, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_SHOW_CURSOR, &DebugAdapterClient::OnDebugShowCursor, this);
    wxTheApp->Unbind(wxEVT_MENU, &DebugAdapterClient::OnSettings, this, XRCID("lldb_settings"));

    EventNotifier::Get()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &DebugAdapterClient::OnRunToCursor, this,
                                 lldbRunToCursorContextMenuId);
    EventNotifier::Get()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &DebugAdapterClient::OnJumpToCursor, this,
                                 lldbJumpToCursorContextMenuId);

    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &DebugAdapterClient::OnAddWatch, this, lldbAddWatchContextMenuId);

    // Dap events
    m_client.Unbind(wxEVT_DAP_EXITED_EVENT, &DebugAdapterClient::OnDapExited, this);
    m_client.Unbind(wxEVT_DAP_TERMINATED_EVENT, &DebugAdapterClient::OnDapExited, this);
    m_client.Unbind(wxEVT_DAP_INITIALIZED_EVENT, &DebugAdapterClient::OnInitializedEvent, this);
    m_client.Unbind(wxEVT_DAP_LAUNCH_RESPONSE, &DebugAdapterClient::OnLaunchResponse, this);
    m_client.Unbind(wxEVT_DAP_STOPPED_EVENT, &DebugAdapterClient::OnStoppedEvent, this);
    m_client.Unbind(wxEVT_DAP_THREADS_RESPONSE, &DebugAdapterClient::OnThreadsResponse, this);
}

DebugAdapterClient::~DebugAdapterClient() {}

bool DebugAdapterClient::ShowThreadNames() const { return m_showThreadNames; }

wxString DebugAdapterClient::GetFilenameForDisplay(const wxString& fileName) const
{
    if(m_showFileNamesOnly) {
        return wxFileName(fileName).GetFullName();
    } else {
        return fileName;
    }
}

void DebugAdapterClient::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void DebugAdapterClient::CreatePluginMenu(wxMenu* pluginsMenu)
{
    // We want to add an entry in the global settings menu
    // Menu Bar > Settings > LLDB Settings

    // Get the main frame's menubar
    auto mb = clGetManager()->GetMenuBar();
    if(mb) {
        wxMenu* settingsMenu(NULL);
        int menuPos = mb->FindMenu(_("Settings"));
        if(menuPos != wxNOT_FOUND) {
            settingsMenu = mb->GetMenu(menuPos);
            if(settingsMenu) {
                settingsMenu->Append(XRCID("lldb_settings"), _("LLDB Settings..."));
            }
        }
    }
}

void DebugAdapterClient::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(type);
    wxUnusedVar(menu);
}

void DebugAdapterClient::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);

    for(auto editor : editors) {
        editor->GetCtrl()->MarkerDeleteAll(smt_indicator);
    }
}

void DebugAdapterClient::SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno)
{
    stc->MarkerDeleteAll(smt_indicator);
    stc->MarkerAdd(lineno, smt_indicator);
    int caretPos = stc->PositionFromLine(lineno);
    stc->SetSelection(caretPos, caretPos);
    stc->SetCurrentPos(caretPos);
    stc->EnsureCaretVisible();
}

void DebugAdapterClient::OnDebugContinue(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.Skip();

    // call continue
    m_client.Continue();
}

void DebugAdapterClient::OnDebugStart(clDebugEvent& event)
{
    if(event.GetDebuggerName() != DEBUGGER_NAME) {
        event.Skip();
        return;
    }

    clDEBUG() << "LLDB: Initial working directory is restored to" << ::wxGetCwd() << endl;
    {
        // Get the executable to debug
        wxString errMsg;
        ProjectPtr pProject = clCxxWorkspaceST::Get()->FindProjectByName(event.GetProjectName(), errMsg);
        if(!pProject) {
            ::wxMessageBox(wxString() << _("Could not locate project: ") << event.GetProjectName(), "LLDB Debugger",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        DirSaver ds;
        ::wxSetWorkingDirectory(pProject->GetFileName().GetPath());

        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(pProject->GetName(), wxEmptyString);
        if(!bldConf) {
            ::wxMessageBox(wxString() << _("Could not locate the requested build configuration"), "LLDB Debugger",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        // Launch codelite-lldb now.
        // Choose wether we need to debug a local or remote target

        // Determine the executable to debug, working directory and arguments
        EnvSetter env(NULL, NULL, pProject ? pProject->GetName() : wxString(), bldConf->GetName());
        wxString exepath = bldConf->GetCommand();
        wxString args;
        wxString workingDirectory;
        // Get the debugging arguments.
        if(bldConf->GetUseSeparateDebugArgs()) {
            args = bldConf->GetDebugArgs();
        } else {
            args = bldConf->GetCommandArguments();
        }

        workingDirectory = MacroManager::Instance()->Expand(bldConf->GetWorkingDirectory(), m_mgr, pProject->GetName());
        exepath = MacroManager::Instance()->Expand(exepath, m_mgr, pProject->GetName());

        {
            DirSaver ds;
            ::wxSetWorkingDirectory(workingDirectory);

#ifndef __WXMSW__
            workingDirectory = ::wxGetCwd();
#endif

            clDEBUG() << "DAP: Using executable:" << exepath << endl;
            clDEBUG() << "DAP: Working directory:" << workingDirectory << endl;

            if(workingDirectory.empty()) {
                workingDirectory = wxGetCwd();
            }
            wxFileName fn(exepath);
            if(fn.IsRelative()) {
                fn.MakeAbsolute(workingDirectory);
            }
            exepath = fn.GetFullPath();

            //////////////////////////////////////////////////////////////////////
            // Initiate the connection to codelite-lldb
            //////////////////////////////////////////////////////////////////////

            // Reset the client
            m_client.Reset();

            wxBusyCursor cursor;
            // For this demo, we use socket transport. But you may choose
            // to write your own transport that implements the dap::Transport interface
            // This is useful when the user wishes to use stdin/out for communicating with
            // the dap and not over socket
            dap::SocketTransport* transport = new dap::SocketTransport();
            if(!transport->Connect("tcp://127.0.0.1:12345", 10)) {
                wxMessageBox("Failed to connect to DAP server", "CodeLite", wxICON_ERROR | wxOK | wxCENTRE);
                wxDELETE(transport);
                m_client.Reset();
                return;
            }

            // construct new client with the transport
            m_client.SetTransport(transport);
            m_client.Initialize(); // send protocol Initialize request

            wxArrayString command_array = StringUtils::BuildArgv(args);
            command_array.Insert(exepath, 0);

            std::vector<wxString> v{ command_array.begin(), command_array.end() };
            clSYSTEM() << "Starting debugger for command:" << endl;
            clSYSTEM() << v << endl;
            clSYSTEM() << "working directory:" << workingDirectory << endl;
            m_client.Launch(std::move(v), workingDirectory, false);
        }
    }
}

void DebugAdapterClient::OnDapExited(DAPEvent& event)
{
    event.Skip();
    m_client.Reset();

    // Save current perspective before destroying the session
    if(m_isPerspectiveLoaded) {
        m_mgr->SavePerspective("DAP");

        // Restore the old perspective
        m_mgr->LoadPerspective("Default");
        m_isPerspectiveLoaded = false;
    }

    DestroyUI();

    clDEBUG() << "CODELITE>> DAP exited" << endl;

    clDebugEvent e(wxEVT_DEBUG_ENDED);
    EventNotifier::Get()->AddPendingEvent(e);
}

void DebugAdapterClient::OnLaunchResponse(DAPEvent& event)
{
    // Check that the debugee was started successfully
    dap::LaunchResponse* resp = event.GetDapResponse()->As<dap::LaunchResponse>();
    if(resp && !resp->success) {
        // launch failed!
        wxMessageBox("Failed to launch debuggee: " + resp->message, "DAP",
                     wxICON_ERROR | wxOK | wxOK_DEFAULT | wxCENTRE);
        m_client.CallAfter(&dap::Client::Reset);
    } else if(resp) {
        InitializeUI();
        LoadPerspective();

        // Fire CodeLite IDE event indicating that a debug session started
        clDebugEvent cl_event{ wxEVT_DEBUG_STARTED };
        EventNotifier::Get()->AddPendingEvent(cl_event);
    }
}

/// DAP server responded to our `initialize` request
void DebugAdapterClient::OnInitializedEvent(DAPEvent& event)
{
    // got initialized event, place breakpoints and continue
    clDEBUG() << "Got Initialized event" << endl;
    clDEBUG() << "Placing breakpoint at main..." << endl;

    // fetch all breakpoints from CodeLite breakpoints manager
    clDebuggerBreakpoint::Vec_t gdbBps;
    m_mgr->GetAllBreakpoints(gdbBps);

    std::vector<dap::FunctionBreakpoint> function_bps;

    // sort the breakpoints into types
    std::unordered_map<wxString, std::vector<dap::SourceBreakpoint>> source_bps;
    for(auto bp : gdbBps) {
        if(bp.bp_type == BreakpointType::BP_type_break) {
            if(!bp.function_name.empty()) {
                function_bps.push_back({ bp.function_name, wxEmptyString });
            } else {
                if(source_bps.count(bp.file) == 0) {
                    source_bps.insert({ bp.file, {} });
                }
                source_bps[bp.file].push_back({ bp.lineno, wxEmptyString });
            }
        }
    }

    m_client.SetFunctionBreakpoints(function_bps);
    for(auto vt : source_bps) {
        m_client.SetBreakpointsFile(vt.first, vt.second);
    }

    // place all breakpoints
    m_client.ConfigurationDone();
    m_client.Continue();
}

void DebugAdapterClient::OnStoppedEvent(DAPEvent& event)
{
    // got stopped event
    dap::StoppedEvent* stopped_data = event.GetDapEvent()->As<dap::StoppedEvent>();
    if(stopped_data) {
        clSYSTEM() << "Stopped reason:" << stopped_data->reason << endl;
        clSYSTEM() << "All threads stopped:" << stopped_data->allThreadsStopped << endl;
        clSYSTEM() << "Stopped thread ID:" << stopped_data->threadId
                   << "(active thread ID:" << m_client.GetActiveThreadId() << ")" << endl;
        m_client.GetThreads();
    }
}

void DebugAdapterClient::OnThreadsResponse(DAPEvent& event)
{
    if(m_threadsView) {
        m_threadsView->UpdateThreads(event);
    }
}

void DebugAdapterClient::OnDebugNext(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    clDEBUG() << "LLDB    >> Next" << endl;
    m_client.Next();
}

void DebugAdapterClient::OnDebugStop(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    clDEBUG() << "LLDB    >> Stop" << endl;
    m_client.Reset();
}

void DebugAdapterClient::OnDebugIsRunning(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.SetAnswer(m_client.IsConnected());
}

void DebugAdapterClient::OnDebugCanInteract(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.SetAnswer(m_client.IsConnected() && m_client.CanInteract());
}

void DebugAdapterClient::OnDebugStepIn(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    m_client.StepIn();
}

void DebugAdapterClient::OnDebugStepOut(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    m_client.StepOut();
}

void DebugAdapterClient::OnIsDebugger(clDebugEvent& event)
{
    event.Skip();
    // register us as a debugger
    event.GetStrings().Add(DEBUGGER_NAME);
}

void DebugAdapterClient::LoadPerspective()
{
    // Save the current persepctive we start debguging
    m_mgr->SavePerspective("Default");

    // Load the LLDB perspective
    m_mgr->LoadPerspective("DAP");
    m_isPerspectiveLoaded = true;

    // Make sure that all the panes are visible
    ShowPane(DAP_MAIN_VIEW, true);

    // Hide the output pane
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane("Output View");
    if(pi.IsOk() && pi.IsShown()) {
        pi.Hide();
    }
    m_mgr->GetDockingManager()->Update();
}

void DebugAdapterClient::ShowPane(const wxString& paneName, bool show)
{
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(paneName);
    if(pi.IsOk()) {
        if(show) {
            if(!pi.IsShown()) {
                pi.Show();
            }
        } else {
            if(pi.IsShown()) {
                pi.Hide();
            }
        }
    }
}

void DebugAdapterClient::DestroyUI()
{
    // Destroy the callstack window
    if(m_threadsView) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(DAP_MAIN_VIEW);
        if(pi.IsOk()) {
            m_mgr->GetDockingManager()->DetachPane(m_threadsView);
        }
        m_threadsView->Destroy();
        m_threadsView = NULL;
    }
    ClearDebuggerMarker();
    m_mgr->GetDockingManager()->Update();
}

void DebugAdapterClient::InitializeUI()
{
    wxWindow* parent = m_mgr->GetDockingManager()->GetManagedWindow();
    if(!m_threadsView) {
        m_threadsView = new DAPMainView(parent, &m_client);
        m_mgr->GetDockingManager()->AddPane(m_threadsView, wxAuiPaneInfo()
                                                               .MinSize(200, 200)
                                                               .Layer(10)
                                                               .Right()
                                                               .Position(1)
                                                               .CloseButton()
                                                               .Caption(DAP_MAIN_VIEW)
                                                               .Name(DAP_MAIN_VIEW));
    }
}

void DebugAdapterClient::OnToggleBreakpoint(clDebugEvent& event)
{
    // Call Skip() here since we want codelite to manage the breakpoint as well ( in term of serialization in the
    // session file )
    CHECK_IS_DAP_CONNECTED();
    event.Skip();

    // FIXME
}

void DebugAdapterClient::DoCleanup()
{
    ClearDebuggerMarker();
    m_client.Reset();
    m_raisOnBpHit = false;
}

void DebugAdapterClient::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    m_client.Reset();
}

void DebugAdapterClient::OnWorkspaceLoaded(wxCommandEvent& event) { event.Skip(); }

void DebugAdapterClient::OnToggleInterrupt(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.Skip();
    m_client.Pause();
}

void DebugAdapterClient::OnBuildStarting(clBuildEvent& event)
{
    if(m_client.IsConnected()) {
        // lldb session is active, prompt the user
        if(::wxMessageBox(_("A debug session is running\nCancel debug session and continue building?"), "CodeLite",
                          wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT | wxCENTER) == wxYES) {
            clDebugEvent dummy;
            OnDebugStop(dummy);
            event.Skip();
        } else {
            // do nothing - this will cancel the build
        }

    } else {
        event.Skip();
    }
}

void DebugAdapterClient::OnAddWatch(wxCommandEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
#if 0
    const auto editor = m_mgr->GetActiveEditor();
    if(!editor) {
        return;
    }

    const auto watchWord = GetWatchWord(*editor);
    if(watchWord.IsEmpty()) {
        return;
    }

    GetLLDB()->AddWatch(watchWord);

    // Refresh the locals view
    GetLLDB()->RequestLocals();
#endif
}

void DebugAdapterClient::OnRunToCursor(wxCommandEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
#if 0
    const auto editor = m_mgr->GetActiveEditor();
    if(!editor) {
        return;
    }

    m_connector.RunTo(editor->GetFileName(), editor->GetCurrentLine() + 1);
#endif
}

void DebugAdapterClient::OnJumpToCursor(wxCommandEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    // FIXME
#if 0
    const auto editor = m_mgr->GetActiveEditor();
    if(!editor) {
        return;
    }

    m_connector.JumpTo(editor->GetFileName(), editor->GetCurrentLine() + 1);
#endif
}

void DebugAdapterClient::OnSettings(wxCommandEvent& event)
{
    event.Skip();
#if 0
    LLDBSettingDialog dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        dlg.Save();
    }
#endif
}

void DebugAdapterClient::OnInitDone(wxCommandEvent& event) { event.Skip(); }

void DebugAdapterClient::OnDebugTooltip(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
#if 0
    // FIXME: use the function ::GetCppExpressionFromPos() to get a better expression
    wxString expression = event.GetString();
    if(expression.IsEmpty())
        return;

    m_connector.EvaluateExpression(expression);
#endif
}

void DebugAdapterClient::OnDebugQuickDebug(clDebugEvent& event)
{
#if 0
    if(!DoInitializeDebugger(event, true,
                             clDebuggerTerminalPOSIX::MakeExeTitle(event.GetExecutableName(), event.GetArguments()))) {
        return;
    }

    LLDBConnectReturnObject retObj;
    LLDBSettings settings;
    settings.Load();
    if(m_connector.Connect(retObj, settings, 5)) {

        // Apply the environment
        EnvSetter env;

        // Get list of breakpoints and add them ( we will apply them later on )
        clDebuggerBreakpoint::Vec_t gdbBps;
        m_mgr->GetAllBreakpoints(gdbBps);

        // remove all breakpoints from previous session
        m_connector.DeleteAllBreakpoints();

        // apply the serialized breakpoints
        // In 'Quick Debug' we stop on main
        m_connector.AddBreakpoint("main");
        m_connector.AddBreakpoints(gdbBps);

        // Setup pivot folder if needed
        SetupPivotFolder(retObj);

        LLDBCommand startCommand;
        startCommand.FillEnvFromMemory();
        startCommand.SetExecutable(event.GetExecutableName());
        startCommand.SetCommandArguments(event.GetArguments());
        startCommand.SetWorkingDirectory(event.GetWorkingDirectory());
        startCommand.SetStartupCommands(event.GetStartupCommands());
        startCommand.SetRedirectTTY(m_debuggerTerminal.GetTty());
        m_connector.Start(startCommand);

    } else {
        // Failed to connect, notify and perform cleanup
        DoCleanup();
        wxString message;
        message << _("Could not connect to codelite-lldb at '") << m_connector.GetConnectString() << "'";
        ::wxMessageBox(message, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
#endif
}

void DebugAdapterClient::OnDebugCoreFile(clDebugEvent& event)
{
    // FIXME
#if 0
    if(event.GetDebuggerName() != LLDB_DEBUGGER_NAME) {
        event.Skip();
        return;
    }

#ifdef __WXMSW__
    ::wxMessageBox(_("Debug core file with LLDB is not supported under Windows"), "CodeLite",
                   wxOK | wxCENTER | wxICON_WARNING);
    return;
#endif

    if(!DoInitializeDebugger(event, false, clDebuggerTerminalPOSIX::MakeCoreTitle(event.GetCoreFile()))) {
        return;
    }

    LLDBConnectReturnObject retObj;
    LLDBSettings settings;
    settings.Load();

    if(m_connector.Connect(retObj, settings, 5)) {

        // Apply the environment
        EnvSetter env;

        // remove all breakpoints from previous session
        m_connector.DeleteAllBreakpoints();

        LLDBCommand startCommand;
        startCommand.FillEnvFromMemory();
        startCommand.SetCommandType(kCommandDebugCoreFile);
        startCommand.SetExecutable(event.GetExecutableName());
        startCommand.SetCorefile(event.GetCoreFile());
        startCommand.SetWorkingDirectory(event.GetWorkingDirectory());
        startCommand.SetRedirectTTY(m_debuggerTerminal.GetTty());
        m_connector.OpenCoreFile(startCommand);
    } else {
        // Failed to connect, notify and perform cleanup
        DoCleanup();
        wxString message;
        message << _("Could not connect to codelite-lldb at '") << m_connector.GetConnectString() << "'";
        ::wxMessageBox(message, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
#endif
}

void DebugAdapterClient::OnDebugAttachToProcess(clDebugEvent& event)
{
#if 0
    if(event.GetDebuggerName() != LLDB_DEBUGGER_NAME) {
        event.Skip();
        return;
    }

#ifdef __WXMSW__
    ::wxMessageBox(_("Attach to process with LLDB is not supported under Windows"), "CodeLite",
                   wxOK | wxCENTER | wxICON_WARNING);
    return;
#endif

    if(!DoInitializeDebugger(event, true, clDebuggerTerminalPOSIX::MakePidTitle(event.GetInt())))
        return;

    LLDBConnectReturnObject retObj;
    LLDBSettings settings;
    settings.Load();

    if(m_connector.Connect(retObj, settings, 5)) {

        // Apply the environment
        EnvSetter env;

        // remove all breakpoints from previous session
        m_connector.DeleteAllBreakpoints();
        LLDBSettings settings;
        settings.Load();

        // Attach to the process
        LLDBCommand command;
        command.SetCommandType(kCommandAttachProcess);
        command.SetProcessID(event.GetInt());
        command.SetSettings(settings);
        m_connector.AttachProcessWithPID(command);

    } else {
        // Failed to connect, notify and perform cleanup
        DoCleanup();
        wxString message;
        message << _("Could not connect to codelite-lldb at '") << m_connector.GetConnectString() << "'";
        ::wxMessageBox(message, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
#endif
}

void DebugAdapterClient::OnDebugDeleteAllBreakpoints(clDebugEvent& event)
{
    event.Skip();
    // FIXME
}

void DebugAdapterClient::OnDebugDisableAllBreakpoints(clDebugEvent& event) { event.Skip(); }

void DebugAdapterClient::OnDebugEnableAllBreakpoints(clDebugEvent& event) { event.Skip(); }

void DebugAdapterClient::OnDebugVOID(clDebugEvent& event) { CHECK_IS_DAP_CONNECTED(); }

void DebugAdapterClient::OnDebugNextInst(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    // FIXME
}

void DebugAdapterClient::OnDebugShowCursor(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    // FIXME
}
