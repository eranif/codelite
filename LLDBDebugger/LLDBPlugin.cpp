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

static LLDBPlugin* thePlugin = NULL;
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
        thePlugin = new LLDBPlugin(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("DAPPlugin"));
    info.SetDescription(_("Debug Adapter Plugin"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

LLDBPlugin::LLDBPlugin(IManager* manager)
    : IPlugin(manager)
    , m_isPerspectiveLoaded(false)
{
    m_longName = _("Debug Adapter Plugin");
    m_shortName = wxT("DAPPlugin");

    // UI events
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &LLDBPlugin::OnIsDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &LLDBPlugin::OnDebugStart, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &LLDBPlugin::OnDebugContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT, &LLDBPlugin::OnDebugNext, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_IN, &LLDBPlugin::OnDebugStepIn, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_OUT, &LLDBPlugin::OnDebugStepOut, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &LLDBPlugin::OnDebugStop, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &LLDBPlugin::OnDebugIsRunning, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_CAN_INTERACT, &LLDBPlugin::OnDebugCanInteract, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &LLDBPlugin::OnToggleBreakpoint, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_INTERRUPT, &LLDBPlugin::OnToggleInterrupt, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTING, &LLDBPlugin::OnBuildStarting, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &LLDBPlugin::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_EXPR_TOOLTIP, &LLDBPlugin::OnDebugTooltip, this);
    EventNotifier::Get()->Bind(wxEVT_QUICK_DEBUG, &LLDBPlugin::OnDebugQuickDebug, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CORE_FILE, &LLDBPlugin::OnDebugCoreFile, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS, &LLDBPlugin::OnDebugDeleteAllBreakpoints, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_ATTACH_TO_PROCESS, &LLDBPlugin::OnDebugAttachToProcess, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS, &LLDBPlugin::OnDebugEnableAllBreakpoints, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS, &LLDBPlugin::OnDebugDisableAllBreakpoints, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT_INST, &LLDBPlugin::OnDebugNextInst, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_I, &LLDBPlugin::OnDebugVOID, this); // not supported

    EventNotifier::Get()->Bind(wxEVT_DBG_UI_SHOW_CURSOR, &LLDBPlugin::OnDebugShowCursor, this);
    wxTheApp->Bind(wxEVT_MENU, &LLDBPlugin::OnSettings, this, XRCID("lldb_settings"));

    EventNotifier::Get()->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnRunToCursor, this,
                               lldbRunToCursorContextMenuId);
    EventNotifier::Get()->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnJumpToCursor, this,
                               lldbJumpToCursorContextMenuId);

    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnAddWatch, this, lldbAddWatchContextMenuId);

    dap::Initialize(); // register all dap objects

    m_client.Bind(wxEVT_DAP_EXITED_EVENT, &LLDBPlugin::OnDapExited, this);
    m_client.Bind(wxEVT_DAP_TERMINATED_EVENT, &LLDBPlugin::OnDapExited, this);
    m_client.Bind(wxEVT_DAP_INITIALIZED_EVENT, &LLDBPlugin::OnInitializedEvent, this);
    m_client.Bind(wxEVT_DAP_LAUNCH_RESPONSE, &LLDBPlugin::OnLaunchResponse, this);
    m_client.Bind(wxEVT_DAP_STOPPED_EVENT, &LLDBPlugin::OnStoppedEvent, this);
    m_client.Bind(wxEVT_DAP_THREADS_RESPONSE, &LLDBPlugin::OnThreadsResponse, this);
}

void LLDBPlugin::UnPlug()
{
    DestroyUI();

    // UI events
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &LLDBPlugin::OnIsDebugger, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &LLDBPlugin::OnDebugStart, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CONTINUE, &LLDBPlugin::OnDebugContinue, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT, &LLDBPlugin::OnDebugNext, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STOP, &LLDBPlugin::OnDebugStop, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_RUNNING, &LLDBPlugin::OnDebugIsRunning, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_CAN_INTERACT, &LLDBPlugin::OnDebugCanInteract, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_IN, &LLDBPlugin::OnDebugStepIn, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_OUT, &LLDBPlugin::OnDebugStepOut, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &LLDBPlugin::OnToggleBreakpoint, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_INTERRUPT, &LLDBPlugin::OnToggleInterrupt, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTING, &LLDBPlugin::OnBuildStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &LLDBPlugin::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_EXPR_TOOLTIP, &LLDBPlugin::OnDebugTooltip, this);
    EventNotifier::Get()->Unbind(wxEVT_QUICK_DEBUG, &LLDBPlugin::OnDebugQuickDebug, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CORE_FILE, &LLDBPlugin::OnDebugCoreFile, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS, &LLDBPlugin::OnDebugDeleteAllBreakpoints, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_ATTACH_TO_PROCESS, &LLDBPlugin::OnDebugAttachToProcess, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS, &LLDBPlugin::OnDebugEnableAllBreakpoints, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS, &LLDBPlugin::OnDebugDisableAllBreakpoints, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_I, &LLDBPlugin::OnDebugVOID, this); // Not supported
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT_INST, &LLDBPlugin::OnDebugNextInst, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_SHOW_CURSOR, &LLDBPlugin::OnDebugShowCursor, this);
    wxTheApp->Unbind(wxEVT_MENU, &LLDBPlugin::OnSettings, this, XRCID("lldb_settings"));

    EventNotifier::Get()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnRunToCursor, this,
                                 lldbRunToCursorContextMenuId);
    EventNotifier::Get()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnJumpToCursor, this,
                                 lldbJumpToCursorContextMenuId);

    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnAddWatch, this, lldbAddWatchContextMenuId);

    // Dap events
    m_client.Unbind(wxEVT_DAP_EXITED_EVENT, &LLDBPlugin::OnDapExited, this);
    m_client.Unbind(wxEVT_DAP_TERMINATED_EVENT, &LLDBPlugin::OnDapExited, this);
    m_client.Unbind(wxEVT_DAP_INITIALIZED_EVENT, &LLDBPlugin::OnInitializedEvent, this);
    m_client.Unbind(wxEVT_DAP_LAUNCH_RESPONSE, &LLDBPlugin::OnLaunchResponse, this);
    m_client.Unbind(wxEVT_DAP_STOPPED_EVENT, &LLDBPlugin::OnStoppedEvent, this);
    m_client.Unbind(wxEVT_DAP_THREADS_RESPONSE, &LLDBPlugin::OnThreadsResponse, this);
}

LLDBPlugin::~LLDBPlugin() {}

bool LLDBPlugin::ShowThreadNames() const { return m_showThreadNames; }

wxString LLDBPlugin::GetFilenameForDisplay(const wxString& fileName) const
{
    if(m_showFileNamesOnly) {
        return wxFileName(fileName).GetFullName();
    } else {
        return fileName;
    }
}

void LLDBPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void LLDBPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
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

void LLDBPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(type);
    wxUnusedVar(menu);
}

void LLDBPlugin::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);

    for(auto editor : editors) {
        editor->GetCtrl()->MarkerDeleteAll(smt_indicator);
    }
}

void LLDBPlugin::SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno)
{
    stc->MarkerDeleteAll(smt_indicator);
    stc->MarkerAdd(lineno, smt_indicator);
    int caretPos = stc->PositionFromLine(lineno);
    stc->SetSelection(caretPos, caretPos);
    stc->SetCurrentPos(caretPos);
    stc->EnsureCaretVisible();
}

void LLDBPlugin::OnDebugContinue(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.Skip();

    // call continue
    m_client.Continue();
}

void LLDBPlugin::OnDebugStart(clDebugEvent& event)
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

void LLDBPlugin::OnDapExited(DAPEvent& event)
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

void LLDBPlugin::OnLaunchResponse(DAPEvent& event)
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
void LLDBPlugin::OnInitializedEvent(DAPEvent& event)
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

void LLDBPlugin::OnStoppedEvent(DAPEvent& event)
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

void LLDBPlugin::OnThreadsResponse(DAPEvent& event)
{
    if(m_threadsView) {
        m_threadsView->UpdateThreads(event);
    }
}

void LLDBPlugin::OnDebugNext(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    clDEBUG() << "LLDB    >> Next" << endl;
    m_client.Next();
}

void LLDBPlugin::OnDebugStop(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    clDEBUG() << "LLDB    >> Stop" << endl;
    m_client.Reset();
}

void LLDBPlugin::OnDebugIsRunning(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.SetAnswer(m_client.IsConnected());
}

void LLDBPlugin::OnDebugCanInteract(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.SetAnswer(m_client.IsConnected() && m_client.CanInteract());
}

void LLDBPlugin::OnDebugStepIn(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    m_client.StepIn();
}

void LLDBPlugin::OnDebugStepOut(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    m_client.StepOut();
}

void LLDBPlugin::OnIsDebugger(clDebugEvent& event)
{
    event.Skip();
    // register us as a debugger
    event.GetStrings().Add(DEBUGGER_NAME);
}

void LLDBPlugin::LoadPerspective()
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

void LLDBPlugin::ShowPane(const wxString& paneName, bool show)
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

void LLDBPlugin::DestroyUI()
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

void LLDBPlugin::InitializeUI()
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

void LLDBPlugin::OnToggleBreakpoint(clDebugEvent& event)
{
    // Call Skip() here since we want codelite to manage the breakpoint as well ( in term of serialization in the
    // session file )
    CHECK_IS_DAP_CONNECTED();
    event.Skip();

    // FIXME
}

void LLDBPlugin::DoCleanup()
{
    ClearDebuggerMarker();
    m_client.Reset();
    m_stopReasonPrompted = false;
    m_raisOnBpHit = false;
}

void LLDBPlugin::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    m_client.Reset();
}

void LLDBPlugin::OnWorkspaceLoaded(wxCommandEvent& event) { event.Skip(); }

void LLDBPlugin::OnToggleInterrupt(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    event.Skip();
    m_client.Pause();
}

void LLDBPlugin::OnBuildStarting(clBuildEvent& event)
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

void LLDBPlugin::OnAddWatch(wxCommandEvent& event)
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

void LLDBPlugin::OnRunToCursor(wxCommandEvent& event)
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

void LLDBPlugin::OnJumpToCursor(wxCommandEvent& event)
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

void LLDBPlugin::OnSettings(wxCommandEvent& event)
{
    event.Skip();
#if 0
    LLDBSettingDialog dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        dlg.Save();
    }
#endif
}

void LLDBPlugin::OnInitDone(wxCommandEvent& event) { event.Skip(); }

void LLDBPlugin::OnDebugTooltip(clDebugEvent& event)
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

void LLDBPlugin::OnDebugQuickDebug(clDebugEvent& event)
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

void LLDBPlugin::OnDebugCoreFile(clDebugEvent& event)
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

void LLDBPlugin::OnDebugAttachToProcess(clDebugEvent& event)
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

void LLDBPlugin::OnDebugDeleteAllBreakpoints(clDebugEvent& event)
{
    event.Skip();
    // FIXME
}

void LLDBPlugin::OnDebugDisableAllBreakpoints(clDebugEvent& event) { event.Skip(); }

void LLDBPlugin::OnDebugEnableAllBreakpoints(clDebugEvent& event) { event.Skip(); }

void LLDBPlugin::OnDebugVOID(clDebugEvent& event) { CHECK_IS_DAP_CONNECTED(); }

void LLDBPlugin::OnDebugNextInst(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    // FIXME
}

void LLDBPlugin::OnDebugShowCursor(clDebugEvent& event)
{
    CHECK_IS_DAP_CONNECTED();
    // FIXME
}
