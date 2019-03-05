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

#include "FolderMappingDlg.h"
#include "LLDBCallStack.h"
#include "LLDBLocalsView.h"
#include "LLDBOutputView.h"
#include "LLDBPlugin.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBFormat.h"
#include "LLDBProtocol/LLDBSettings.h"
#include "LLDBSettingDialog.h"
#include "LLDBThreadsView.h"
#include "LLDBTooltip.h"
#include "bookmark_manager.h"
#include "clcommandlineparser.h"
#include "console_frame.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "JSON.h"
#include <wx/aui/framemanager.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/platinfo.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

static LLDBPlugin* thePlugin = NULL;

#define LLDB_DEBUGGER_NAME "LLDB Debugger"
#define LLDB_CALLSTACK_PANE_NAME "LLDB Callstack"
#define LLDB_BREAKPOINTS_PANE_NAME "LLDB Breakpoints"
#define LLDB_LOCALS_PANE_NAME "LLDB Locals"
#define LLDB_THREADS_PANE_NAME "LLDB Threads"

namespace
{

// Reusing gdb ids so global debugger menu and accelerators work.
const int lldbRunToCursorContextMenuId = XRCID("dbg_run_to_cursor");
const int lldbJumpToCursorContextMenuId = XRCID("dbg_jump_cursor");

const int lldbAddWatchContextMenuId = XRCID("lldb_add_watch");

wxString GetWatchWord(IEditor& editor)
{
    auto word = editor.GetSelection();
    if(word.IsEmpty()) { word = editor.GetWordAtCaret(); }

    // Remove leading and trailing whitespace.
    word.Trim(true);
    word.Trim(false);

    return word;
}

} // namespace

#define CHECK_IS_LLDB_SESSION()    \
    if(!m_connector.IsRunning()) { \
        event.Skip();              \
        return;                    \
    }

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new LLDBPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("LLDBDebuggerPlugin"));
    info.SetDescription(_("LLDB Debugger for CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

LLDBPlugin::LLDBPlugin(IManager* manager)
    : IPlugin(manager)
    , m_callstack(NULL)
    , m_breakpointsView(NULL)
    , m_localsView(NULL)
    , m_threadsView(NULL)
    , m_stopReasonPrompted(false)
    , m_raisOnBpHit(false)
    , m_tooltip(NULL)
    , m_isPerspectiveLoaded(false)
{
    m_longName = _("LLDB Debugger for CodeLite");
    m_shortName = wxT("LLDBDebuggerPlugin");

    m_connector.Bind(wxEVT_LLDB_STARTED, &LLDBPlugin::OnLLDBStarted, this);
    m_connector.Bind(wxEVT_LLDB_CRASHED, &LLDBPlugin::OnLLDBCrashed, this);
    m_connector.Bind(wxEVT_LLDB_EXITED, &LLDBPlugin::OnLLDBExited, this);
    m_connector.Bind(wxEVT_LLDB_STOPPED, &LLDBPlugin::OnLLDBStopped, this);
    m_connector.Bind(wxEVT_LLDB_RUNNING, &LLDBPlugin::OnLLDBRunning, this);
    m_connector.Bind(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY, &LLDBPlugin::OnLLDBStoppedOnEntry, this);
    m_connector.Bind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBPlugin::OnLLDBDeletedAllBreakpoints, this);
    m_connector.Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBPlugin::OnLLDBBreakpointsUpdated, this);
    m_connector.Bind(wxEVT_LLDB_EXPRESSION_EVALUATED, &LLDBPlugin::OnLLDBExpressionEvaluated, this);
    m_connector.Bind(wxEVT_LLDB_LAUNCH_SUCCESS, &LLDBPlugin::OnLLDBLaunchSuccess, this);

    // UI events
    EventNotifier::Get()->Connect(wxEVT_DBG_IS_PLUGIN_DEBUGGER, clDebugEventHandler(LLDBPlugin::OnIsDebugger), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_START, clDebugEventHandler(LLDBPlugin::OnDebugStart), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_CONTINUE, clDebugEventHandler(LLDBPlugin::OnDebugContinue), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_NEXT, clDebugEventHandler(LLDBPlugin::OnDebugNext), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_STEP_IN, clDebugEventHandler(LLDBPlugin::OnDebugStepIn), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_STEP_OUT, clDebugEventHandler(LLDBPlugin::OnDebugStepOut), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_STOP, clDebugEventHandler(LLDBPlugin::OnDebugStop), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_IS_RUNNING, clDebugEventHandler(LLDBPlugin::OnDebugIsRunning), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_CAN_INTERACT, clDebugEventHandler(LLDBPlugin::OnDebugCanInteract), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, clDebugEventHandler(LLDBPlugin::OnToggleBreakpoint),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_INTERRUPT, clDebugEventHandler(LLDBPlugin::OnToggleInterrupt), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTING, clBuildEventHandler(LLDBPlugin::OnBuildStarting), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(LLDBPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_EXPR_TOOLTIP, clDebugEventHandler(LLDBPlugin::OnDebugTooltip), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_QUICK_DEBUG, clDebugEventHandler(LLDBPlugin::OnDebugQuickDebug), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_CORE_FILE, clDebugEventHandler(LLDBPlugin::OnDebugCoreFile), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,
                                  clDebugEventHandler(LLDBPlugin::OnDebugDeleteAllBreakpoints), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_ATTACH_TO_PROCESS,
                                  clDebugEventHandler(LLDBPlugin::OnDebugAttachToProcess), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS,
                                  clDebugEventHandler(LLDBPlugin::OnDebugEnableAllBreakpoints), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS,
                                  clDebugEventHandler(LLDBPlugin::OnDebugDisableAllBreakpoints), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_NEXT_INST, clDebugEventHandler(LLDBPlugin::OnDebugNextInst), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_I, &LLDBPlugin::OnDebugVOID, this); // not supported

    EventNotifier::Get()->Connect(wxEVT_DBG_UI_SHOW_CURSOR, clDebugEventHandler(LLDBPlugin::OnDebugShowCursor), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_TOOLTIP_DESTROY, &LLDBPlugin::OnDestroyTip, this);
    wxTheApp->Bind(wxEVT_MENU, &LLDBPlugin::OnSettings, this, XRCID("lldb_settings"));

    EventNotifier::Get()->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnRunToCursor, this,
                               lldbRunToCursorContextMenuId);
    EventNotifier::Get()->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnJumpToCursor, this,
                               lldbJumpToCursorContextMenuId);

    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnAddWatch, this, lldbAddWatchContextMenuId);
    LLDBFormat::Initialise();
}

void LLDBPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_TOOLTIP_DESTROY, &LLDBPlugin::OnDestroyTip, this);
    m_connector.StopDebugServer();
    DestroyUI();

    m_connector.Unbind(wxEVT_LLDB_STARTED, &LLDBPlugin::OnLLDBStarted, this);
    m_connector.Unbind(wxEVT_LLDB_CRASHED, &LLDBPlugin::OnLLDBCrashed, this);
    m_connector.Unbind(wxEVT_LLDB_EXITED, &LLDBPlugin::OnLLDBExited, this);
    m_connector.Unbind(wxEVT_LLDB_STOPPED, &LLDBPlugin::OnLLDBStopped, this);
    m_connector.Unbind(wxEVT_LLDB_RUNNING, &LLDBPlugin::OnLLDBRunning, this);
    m_connector.Unbind(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY, &LLDBPlugin::OnLLDBStoppedOnEntry, this);
    m_connector.Unbind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBPlugin::OnLLDBDeletedAllBreakpoints, this);
    m_connector.Unbind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBPlugin::OnLLDBBreakpointsUpdated, this);
    m_connector.Unbind(wxEVT_LLDB_EXPRESSION_EVALUATED, &LLDBPlugin::OnLLDBExpressionEvaluated, this);
    m_connector.Unbind(wxEVT_LLDB_LAUNCH_SUCCESS, &LLDBPlugin::OnLLDBLaunchSuccess, this);

    // UI events
    EventNotifier::Get()->Disconnect(wxEVT_DBG_IS_PLUGIN_DEBUGGER, clDebugEventHandler(LLDBPlugin::OnIsDebugger), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_START, clDebugEventHandler(LLDBPlugin::OnDebugStart), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_CONTINUE, clDebugEventHandler(LLDBPlugin::OnDebugContinue), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_NEXT, clDebugEventHandler(LLDBPlugin::OnDebugNext), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_STOP, clDebugEventHandler(LLDBPlugin::OnDebugStop), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_IS_RUNNING, clDebugEventHandler(LLDBPlugin::OnDebugIsRunning), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_CAN_INTERACT, clDebugEventHandler(LLDBPlugin::OnDebugCanInteract), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_STEP_IN, clDebugEventHandler(LLDBPlugin::OnDebugStepIn), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_STEP_OUT, clDebugEventHandler(LLDBPlugin::OnDebugStepOut), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_TOGGLE_BREAKPOINT,
                                     clDebugEventHandler(LLDBPlugin::OnToggleBreakpoint), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_INTERRUPT, clDebugEventHandler(LLDBPlugin::OnToggleInterrupt), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTING, clBuildEventHandler(LLDBPlugin::OnBuildStarting), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(LLDBPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_EXPR_TOOLTIP, clDebugEventHandler(LLDBPlugin::OnDebugTooltip), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_QUICK_DEBUG, clDebugEventHandler(LLDBPlugin::OnDebugQuickDebug), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_CORE_FILE, clDebugEventHandler(LLDBPlugin::OnDebugCoreFile), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,
                                     clDebugEventHandler(LLDBPlugin::OnDebugDeleteAllBreakpoints), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_ATTACH_TO_PROCESS,
                                     clDebugEventHandler(LLDBPlugin::OnDebugAttachToProcess), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS,
                                     clDebugEventHandler(LLDBPlugin::OnDebugEnableAllBreakpoints), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS,
                                     clDebugEventHandler(LLDBPlugin::OnDebugDisableAllBreakpoints), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_I, &LLDBPlugin::OnDebugVOID, this); // Not supported
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_NEXT_INST, clDebugEventHandler(LLDBPlugin::OnDebugNextInst), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_SHOW_CURSOR, clDebugEventHandler(LLDBPlugin::OnDebugShowCursor), NULL,
                                     this);
    wxTheApp->Unbind(wxEVT_MENU, &LLDBPlugin::OnSettings, this, XRCID("lldb_settings"));

    EventNotifier::Get()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnRunToCursor, this,
                                 lldbRunToCursorContextMenuId);
    EventNotifier::Get()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnJumpToCursor, this,
                                 lldbJumpToCursorContextMenuId);

    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBPlugin::OnAddWatch, this, lldbAddWatchContextMenuId);
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
    wxMenuBar* mb = pluginsMenu->GetMenuBar();
    if(mb) {
        wxMenu* settingsMenu(NULL);
        int menuPos = mb->FindMenu(_("Settings"));
        if(menuPos != wxNOT_FOUND) {
            settingsMenu = mb->GetMenu(menuPos);
            if(settingsMenu) { settingsMenu->Append(XRCID("lldb_settings"), _("LLDB Settings...")); }
        }
    }
}

void LLDBPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(type);

    if(!m_connector.IsRunning()) { return; }

    const auto editor = m_mgr->GetActiveEditor();
    if(!editor) { return; }

    size_t numberOfMenuItems = 0;

    if(m_connector.IsCanInteract()) {
        menu->Prepend(lldbJumpToCursorContextMenuId, wxT("Jump to Caret Line"));
        ++numberOfMenuItems;

        menu->Prepend(lldbRunToCursorContextMenuId, wxT("Run to Caret Line"));
        ++numberOfMenuItems;
    }

    auto word = GetWatchWord(*editor);
    if(word.Contains("\n")) {
        // Don't create massive context menu
        word.Clear();
    }

    // Truncate the word
    if(word.length() > 20) {
        word.Truncate(20);
        word << "...";
    }

    if(!word.IsEmpty()) {
        const auto menuItemText = wxString(wxT("Add Watch")) << wxT(" '") << word << wxT("'");
        menu->Prepend(lldbAddWatchContextMenuId, menuItemText);
        ++numberOfMenuItems;
    }

    if(numberOfMenuItems > 0) { menu->InsertSeparator(numberOfMenuItems); }
}

void LLDBPlugin::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);
    IEditor::List_t::iterator iter = editors.begin();
    for(; iter != editors.end(); ++iter) {
        (*iter)->GetCtrl()->MarkerDeleteAll(smt_indicator);
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

void LLDBPlugin::TerminateTerminal()
{
#ifdef __WXGTK__
    if(m_debuggerTerminal.GetTty().StartsWith("/tmp/pts")) {
        // this is a fake symlink - remove it
        ::unlink(m_debuggerTerminal.GetTty().mb_str(wxConvUTF8).data());
    }
#endif
    m_debuggerTerminal.Clear();
}

void LLDBPlugin::OnDebugContinue(clDebugEvent& event)
{
    event.Skip();
    if(m_connector.IsRunning()) {
        // we are the active debugger
        CL_DEBUG("CODELITE>> continue...");
        m_connector.Continue();
        event.Skip(false);
    }
}

void LLDBPlugin::OnDebugStart(clDebugEvent& event)
{
    if(event.GetDebuggerName() != LLDB_DEBUGGER_NAME) {
        event.Skip();
        return;
    }

    CL_DEBUG("LLDB: Initial working directory is restored to: " + ::wxGetCwd());
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

        // Load LLDB settings
        LLDBSettings settings;
        settings.Load();

        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(pProject->GetName(), wxEmptyString);
        if(!bldConf) {
            ::wxMessageBox(wxString() << _("Could not locate the requested buid configuration"), "LLDB Debugger",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        // Launch codelite-lldb now.
        // Choose wether we need to debug a local or remote target

        // Honour the project settings
        if(bldConf->GetIsDbgRemoteTarget()) {
            long nPort(wxNOT_FOUND);
            bldConf->GetDbgHostPort().ToCLong(&nPort);
            settings.SetProxyIp(bldConf->GetDbgHostName());
            settings.SetProxyPort(nPort);
            settings.EnableFlag(kLLDBOptionUseRemoteProxy, true);
        }

        if(!settings.IsUsingRemoteProxy()) {
            // Not using a remote proxy, launch the debug server
            if(!m_connector.LaunchLocalDebugServer(settings.GetDebugserver())) { return; }
        }

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

        workingDirectory = ::ExpandVariables(bldConf->GetWorkingDirectory(), pProject, m_mgr->GetActiveEditor());
        exepath = ::ExpandVariables(exepath, pProject, m_mgr->GetActiveEditor());

        {
            DirSaver ds;
            ::wxSetWorkingDirectory(workingDirectory);
            wxFileName execToDebug(exepath);
            if(execToDebug.IsRelative()) { execToDebug.MakeAbsolute(); }

            //////////////////////////////////////////////////////////////////////
            // Launch terminal for IO redirection
            //////////////////////////////////////////////////////////////////////
            TerminateTerminal();

            bool isWindows = wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS;
            if(!bldConf->IsGUIProgram() && !isWindows) {
                m_debuggerTerminal.Launch(clDebuggerTerminalPOSIX::MakeExeTitle(execToDebug.GetFullPath(), args));

                if(m_debuggerTerminal.IsValid()) {
                    CL_DEBUG("Successfully launched terminal %s", m_debuggerTerminal.GetTty());

                } else {
                    // Failed to launch it...
                    DoCleanup();
                    ::wxMessageBox(_("Failed to start terminal for debugger"), "CodeLite",
                                   wxICON_ERROR | wxOK | wxCENTER);
                    return;
                }
            }

            if(!isWindows) { workingDirectory = ::wxGetCwd(); }

            CL_DEBUG("LLDB: Using executable : " + execToDebug.GetFullPath());
            CL_DEBUG("LLDB: Working directory: " + workingDirectory);

            //////////////////////////////////////////////////////////////////////
            // Initiate the connection to codelite-lldb
            //////////////////////////////////////////////////////////////////////

            LLDBConnectReturnObject retObj;
            if(m_connector.Connect(retObj, settings, 5)) {

                // Get list of breakpoints and add them ( we will apply them later on )
                BreakpointInfo::Vec_t gdbBps;
                m_mgr->GetAllBreakpoints(gdbBps);

                // remove all breakpoints from previous session
                m_connector.DeleteAllBreakpoints();

                // apply the serialized breakpoints
                m_connector.AddBreakpoints(gdbBps);

                // Setup pivot folder if needed
                SetupPivotFolder(retObj);

                LLDBCommand startCommand;
                startCommand.FillEnvFromMemory();

                // If the current platform is Windows, use the executable as it appears in the project settings
                startCommand.SetExecutable(isWindows ? exepath : execToDebug.GetFullPath());

                startCommand.SetCommandArguments(args);
                // Since we called 'wxSetWorkingDirectory' earlier, wxGetCwd() should give use the
                // correct working directory for the debugger
                startCommand.SetWorkingDirectory(workingDirectory);
                startCommand.SetRedirectTTY(m_debuggerTerminal.GetTty());
                m_connector.Start(startCommand);

                clDebugEvent cl_event(wxEVT_DEBUG_STARTED);
                EventNotifier::Get()->AddPendingEvent(cl_event);

            } else {
                // Failed to connect, notify and perform cleanup
                DoCleanup();
                wxString message;
                message << _("Could not connect to codelite-lldb at '")
                        << (settings.IsUsingRemoteProxy() ? settings.GetTcpConnectString()
                                                          : m_connector.GetConnectString())
                        << "'";
                ::wxMessageBox(message, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }
        }
    }
    CL_DEBUG("LLDB: Working directory is restored to: " + ::wxGetCwd());
}

void LLDBPlugin::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    m_connector.SetGoingDown(true);

    // Stop the debugger ( do not notify about it, since we are in the handler...)
    m_connector.Cleanup();

    // Save current perspective before destroying the session
    if(m_isPerspectiveLoaded) {
        m_mgr->SavePerspective("LLDB-debugger");

        // Restore the old perspective
        m_mgr->LoadPerspective("Default");
        m_isPerspectiveLoaded = false;
    }

    DestroyUI();

    // Reset various state variables
    DoCleanup();

    CL_DEBUG("CODELITE>> LLDB exited");

    // Also notify codelite's event
    clDebugEvent e2(wxEVT_DEBUG_ENDED);
    EventNotifier::Get()->AddPendingEvent(e2);

    {
        clDebugEvent e(wxEVT_DEBUG_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);
    }
}

void LLDBPlugin::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();

    const auto settings = LLDBSettings().Load();
    m_showThreadNames = settings.HasFlag(kLLDBOptionShowThreadNames);
    m_showFileNamesOnly = settings.HasFlag(kLLDBOptionShowFileNamesOnly);

    InitializeUI();
    LoadLLDBPerspective();

    // If this is a normal debug session, a start notification
    // should follow a 'Run' command
    switch(event.GetSessionType()) {
    case kDebugSessionTypeCore:
        CL_DEBUG("CODELITE>> LLDB started (core file)");
        break;

    case kDebugSessionTypeAttach: {
        m_raisOnBpHit = settings.IsRaiseWhenBreakpointHit();
        CL_DEBUG("CODELITE>> LLDB started (attached)");
        m_connector.SetAttachedToProcess(event.GetSessionType() == kDebugSessionTypeAttach);
        // m_connector.Continue();
        break;
    }
    case kDebugSessionTypeNormal: {
        m_raisOnBpHit = settings.IsRaiseWhenBreakpointHit();
        CL_DEBUG("CODELITE>> LLDB started (normal)");
        m_connector.Run();
        break;
    }
    }

    // notify plugins that the debugger just started
    {
        clDebugEvent eventStarted(wxEVT_DEBUG_STARTED);
        eventStarted.SetClientData(NULL);
        EventNotifier::Get()->ProcessEvent(eventStarted);
    }
}

void LLDBPlugin::OnLLDBStopped(LLDBEvent& event)
{
    event.Skip();
    CL_DEBUGS(wxString() << "CODELITE>> LLDB stopped at " << event.GetFileName() << ":" << event.GetLinenumber());
    m_connector.SetCanInteract(true);

    if(event.GetInterruptReason() == kInterruptReasonNone) {

        if(m_raisOnBpHit) { EventNotifier::Get()->TopFrame()->Raise(); }

        // Mark the debugger line / file
        IEditor* editor = m_mgr->FindEditor(event.GetFileName());
        if(!editor && wxFileName::Exists(event.GetFileName())) {
            // Try to open the editor
            editor = m_mgr->OpenFile(event.GetFileName(), "", event.GetLinenumber() - 1);
        }

        if(editor) {
            // select it first
            if(editor != m_mgr->GetActiveEditor()) {
                m_mgr->SelectPage(editor->GetCtrl());

            } else {
                // just make sure that the page has the focus
                editor->SetActive();
            }

            // clear the markers
            ClearDebuggerMarker();
            SetDebuggerMarker(editor->GetCtrl(), event.GetLinenumber() - 1);

        } else {
            ClearDebuggerMarker();
        }

        // request for local variables
        m_connector.RequestLocals();

        wxString message;
        if(!m_stopReasonPrompted && event.ShouldPromptStopReason(message)) {
            m_stopReasonPrompted = true; // show this message only once per debug session
            wxString msg;
            msg << "Program stopped\nStop reason: " << message;
            ::wxMessageBox(msg, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        }

    } else if(event.GetInterruptReason() == kInterruptReasonApplyBreakpoints) {
        CL_DEBUG("Applying breakpoints and continue...");
        m_connector.ApplyBreakpoints();
        m_connector.Continue();

    } else if(event.GetInterruptReason() == kInterruptReasonDeleteAllBreakpoints) {
        CL_DEBUG("Deleting all breakpoints");
        m_connector.DeleteAllBreakpoints();
        m_connector.Continue();

    } else if(event.GetInterruptReason() == kInterruptReasonDeleteBreakpoint) {
        CL_DEBUG("Deleting all pending deletion breakpoints");
        m_connector.DeleteBreakpoints();
        m_connector.Continue();

    } else if(event.GetInterruptReason() == kInterruptReasonDetaching) {
        CL_DEBUG("Detaching from process");
        m_connector.Detach();
    }
}

void LLDBPlugin::OnLLDBStoppedOnEntry(LLDBEvent& event)
{
    event.Skip();
    m_connector.SetCanInteract(true);
    m_connector.SetIsRunning(true);

    CL_DEBUG("CODELITE>> Applying breakpoints...");
    m_connector.ApplyBreakpoints();
    CL_DEBUG("CODELITE>> continue...");
    m_connector.Continue();
}

void LLDBPlugin::OnDebugNext(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    CL_DEBUG("LLDB    >> Next");
    m_connector.Next();
}

void LLDBPlugin::OnDebugStop(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    CL_DEBUG("LLDB    >> Stop");
    m_connector.Stop();
}

void LLDBPlugin::OnDebugIsRunning(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    event.SetAnswer(m_connector.IsRunning());
}

void LLDBPlugin::OnDebugCanInteract(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    event.SetAnswer(m_connector.IsCanInteract());
}

void LLDBPlugin::OnDebugStepIn(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    m_connector.StepIn();
}

void LLDBPlugin::OnDebugStepOut(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    m_connector.StepOut();
}

void LLDBPlugin::OnIsDebugger(clDebugEvent& event)
{
    event.Skip();
    // register us as a debugger
    event.GetStrings().Add(LLDB_DEBUGGER_NAME);
}

void LLDBPlugin::LoadLLDBPerspective()
{
    // Save the current persepctive we start debguging
    m_mgr->SavePerspective("Default");

    // Load the LLDB perspective
    m_mgr->LoadPerspective("LLDB-Debugger");
    m_isPerspectiveLoaded = true;

    // Make sure that all the panes are visible
    ShowLLDBPane(LLDB_CALLSTACK_PANE_NAME);
    ShowLLDBPane(LLDB_BREAKPOINTS_PANE_NAME);
    ShowLLDBPane(LLDB_LOCALS_PANE_NAME);
    ShowLLDBPane(LLDB_THREADS_PANE_NAME);

    // Hide the output pane
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane("Output View");
    if(pi.IsOk() && pi.IsShown()) { pi.Hide(); }
    m_mgr->GetDockingManager()->Update();
}

void LLDBPlugin::ShowLLDBPane(const wxString& paneName, bool show)
{
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(paneName);
    if(pi.IsOk()) {
        if(show) {
            if(!pi.IsShown()) { pi.Show(); }
        } else {
            if(pi.IsShown()) { pi.Hide(); }
        }
    }
}

void LLDBPlugin::DestroyUI()
{
    // Destroy the callstack window
    if(m_callstack) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(LLDB_CALLSTACK_PANE_NAME);
        if(pi.IsOk()) { m_mgr->GetDockingManager()->DetachPane(m_callstack); }
        m_callstack->Destroy();
        m_callstack = NULL;
    }
    if(m_breakpointsView) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(LLDB_BREAKPOINTS_PANE_NAME);
        if(pi.IsOk()) { m_mgr->GetDockingManager()->DetachPane(m_breakpointsView); }
        m_breakpointsView->Destroy();
        m_breakpointsView = NULL;
    }
    if(m_localsView) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(LLDB_LOCALS_PANE_NAME);
        if(pi.IsOk()) { m_mgr->GetDockingManager()->DetachPane(m_localsView); }
        m_localsView->Destroy();
        m_localsView = NULL;
    }

    if(m_threadsView) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(LLDB_THREADS_PANE_NAME);
        if(pi.IsOk()) { m_mgr->GetDockingManager()->DetachPane(m_threadsView); }
        m_threadsView->Destroy();
        m_threadsView = NULL;
    }
    if(m_tooltip) {
        m_tooltip->Destroy();
        m_tooltip = NULL;
    }
    ClearDebuggerMarker();
    m_mgr->GetDockingManager()->Update();
}

void LLDBPlugin::InitializeUI()
{
    wxWindow* parent = m_mgr->GetDockingManager()->GetManagedWindow();
    if(!m_breakpointsView) {
        m_breakpointsView = new LLDBOutputView(parent, this);
        m_mgr->GetDockingManager()->AddPane(m_breakpointsView, wxAuiPaneInfo()
                                                                   .MinSize(200, 200)
                                                                   .Right()
                                                                   .Position(1)
                                                                   .Layer(10)
                                                                   .CloseButton()
                                                                   .Caption("Breakpoints")
                                                                   .Name(LLDB_BREAKPOINTS_PANE_NAME));
    }
    if(!m_callstack) {
        m_callstack = new LLDBCallStackPane(parent, *this);
        m_mgr->GetDockingManager()->AddPane(m_callstack, wxAuiPaneInfo()
                                                             .MinSize(200, 200)
                                                             .Right()
                                                             .Position(2) // top one
                                                             .Layer(10)   // outer layer
                                                             .CloseButton()
                                                             .Caption("Callstack")
                                                             .Name(LLDB_CALLSTACK_PANE_NAME));
    }

    if(!m_threadsView) {
        m_threadsView = new LLDBThreadsView(parent, this);
        m_mgr->GetDockingManager()->AddPane(
            m_threadsView,
            wxAuiPaneInfo().MinSize(200, 200).Layer(10).Right().Position(1).CloseButton().Caption("Threads").Name(
                LLDB_THREADS_PANE_NAME));
    }

    if(!m_localsView) {
        m_localsView = new LLDBLocalsView(parent, this);
        m_mgr->GetDockingManager()->AddPane(m_localsView,
                                            wxAuiPaneInfo()
                                                .MinSize(200, 200)
                                                .Bottom()
                                                .Layer(5) // outer, but not on the same layer as the left side pane
                                                .Position(0)
                                                .CloseButton()
                                                .Caption("Locals & Watches")
                                                .Name(LLDB_LOCALS_PANE_NAME));
    }
}

void LLDBPlugin::OnLLDBRunning(LLDBEvent& event)
{
    event.Skip();
    m_connector.SetCanInteract(false);

    // When the IDE loses the focus - clear the debugger marker
    ClearDebuggerMarker();
}

void LLDBPlugin::OnToggleBreakpoint(clDebugEvent& event)
{
    // Call Skip() here since we want codelite to manage the breakpoint as well ( in term of serialization in the
    // session file )
    CHECK_IS_LLDB_SESSION();

    // check to see if we are removing a breakpoint or adding one
    LLDBBreakpoint::Ptr_t bp(new LLDBBreakpoint(event.GetFileName(), event.GetInt()));
    IEditor* editor = m_mgr->GetActiveEditor();

    if(editor) {
        // get the marker type set on the line
        int markerType = editor->GetCtrl()->MarkerGet(bp->GetLineNumber() - 1);
        for(size_t type = smt_FIRST_BP_TYPE; type <= smt_LAST_BP_TYPE; ++type) {
            int markerMask = (1 << type);
            if(markerType & markerMask) {
                // removing a breakpoint. "DeleteBreakpoint" will handle the interactive/non-interactive mode
                // of the debugger
                m_connector.MarkBreakpointForDeletion(bp);
                m_connector.DeleteBreakpoints();
                return;
            }
        }

        // if we got here, its a new breakpoint, add it
        // Add the breakpoint to the list of breakpoints
        m_connector.AddBreakpoint(bp->GetFilename(), bp->GetLineNumber());

        // apply it. In case the debugger can not interact with, it will be interrupted and the interrupt reason
        // will be set to ApplyBreakpoints
        m_connector.ApplyBreakpoints();
    }
}

void LLDBPlugin::DoCleanup()
{
    ClearDebuggerMarker();
    TerminateTerminal();
    m_connector.StopDebugServer();
    m_stopReasonPrompted = false;
    m_raisOnBpHit = false;
}

void LLDBPlugin::OnLLDBDeletedAllBreakpoints(LLDBEvent& event)
{
    event.Skip();
    m_mgr->DeleteAllBreakpoints();
}

void LLDBPlugin::OnLLDBBreakpointsUpdated(LLDBEvent& event)
{
    event.Skip();
    // update the ui (mainly editors)
    // this is done by replacing the breakpoints list with a new one (the updated one we take from LLDB)
    m_mgr->SetBreakpoints(LLDBBreakpoint::ToBreakpointInfoVector(event.GetBreakpoints()));
}

void LLDBPlugin::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    if(m_connector.IsRunning()) { m_connector.Stop(); }
    m_connector.Cleanup();
}

void LLDBPlugin::OnWorkspaceLoaded(wxCommandEvent& event) { event.Skip(); }

void LLDBPlugin::OnLLDBCrashed(LLDBEvent& event)
{
    event.Skip();
    // Report it as crash only if not going down (i.e. we got an LLDBExit event)
    if(!m_connector.IsGoingDown()) {
        // SetGoingDown() before displaying message box to cope with reentering this function whilst waiting for OK.
        m_connector.SetGoingDown(true);
        ::wxMessageBox(_("LLDB crashed! Terminating debug session"), "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
        OnLLDBExited(event);
    }
}

void LLDBPlugin::OnToggleInterrupt(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    event.Skip();
    CL_DEBUG("CODELITE: interrupting debuggee");
    if(!m_connector.IsCanInteract()) { m_connector.Interrupt(kInterruptReasonNone); }
}

void LLDBPlugin::OnBuildStarting(clBuildEvent& event)
{
    if(m_connector.IsRunning()) {
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
    CHECK_IS_LLDB_SESSION();

    const auto editor = m_mgr->GetActiveEditor();
    if(!editor) { return; }

    const auto watchWord = GetWatchWord(*editor);
    if(watchWord.IsEmpty()) { return; }

    GetLLDB()->AddWatch(watchWord);

    // Refresh the locals view
    GetLLDB()->RequestLocals();
}

void LLDBPlugin::OnRunToCursor(wxCommandEvent& event)
{
    CHECK_IS_LLDB_SESSION();

    const auto editor = m_mgr->GetActiveEditor();
    if(!editor) { return; }

    m_connector.RunTo(editor->GetFileName(), editor->GetCurrentLine() + 1);
}

void LLDBPlugin::OnJumpToCursor(wxCommandEvent& event)
{
    CHECK_IS_LLDB_SESSION();

    const auto editor = m_mgr->GetActiveEditor();
    if(!editor) { return; }

    m_connector.JumpTo(editor->GetFileName(), editor->GetCurrentLine() + 1);
}

void LLDBPlugin::OnSettings(wxCommandEvent& event)
{
    event.Skip();
    LLDBSettingDialog dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) { dlg.Save(); }
}

void LLDBPlugin::OnInitDone(wxCommandEvent& event) { event.Skip(); }

void LLDBPlugin::OnDebugTooltip(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();

    // FIXME: use the function ::GetCppExpressionFromPos() to get a better expression
    wxString expression = event.GetString();
    if(expression.IsEmpty()) return;

    m_connector.EvaluateExpression(expression);
}

void LLDBPlugin::OnLLDBExpressionEvaluated(LLDBEvent& event)
{
    CHECK_IS_LLDB_SESSION();

    // hide any tooltip
    if(!event.GetVariables().empty() && m_mgr->GetActiveEditor()) {
        if(!m_tooltip) { m_tooltip = new LLDBTooltip(this); }
        m_tooltip->Show(event.GetExpression(), event.GetVariables().at(0));
    }
}

void LLDBPlugin::OnDebugQuickDebug(clDebugEvent& event)
{
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
        BreakpointInfo::Vec_t gdbBps;
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
}

void LLDBPlugin::OnDebugCoreFile(clDebugEvent& event)
{
    if(event.GetDebuggerName() != LLDB_DEBUGGER_NAME) {
        event.Skip();
        return;
    }

#ifdef __WXMSW__
    ::wxMessageBox(_("Debug core file with LLDB is not supported under Windows"), "CodeLite",
                   wxOK | wxCENTER | wxICON_WARNING);
    return;
#endif

    if(!DoInitializeDebugger(event, false, clDebuggerTerminalPOSIX::MakeCoreTitle(event.GetCoreFile()))) { return; }

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
}

bool LLDBPlugin::DoInitializeDebugger(clDebugEvent& event, bool redirectOutput, const wxString& terminalTitle)
{
    if(event.GetDebuggerName() != LLDB_DEBUGGER_NAME) {
        event.Skip();
        return false;
    }

    if(m_connector.IsRunning()) {
        // Another debug session is already in progress
        ::wxMessageBox(_("Another debug session is already in progress. Please stop it first"), "CodeLite",
                       wxOK | wxCENTER | wxICON_WARNING);
        return false;
    }

    TerminateTerminal();

    // If terminal is required, launch it now
    bool isWindows = wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS;
    if(redirectOutput && !isWindows) {
        m_debuggerTerminal.Launch(terminalTitle);

        if(m_debuggerTerminal.IsValid()) {
            CL_DEBUG("Successfully launched terminal");

        } else {
            // Failed to launch it...
            DoCleanup();
            ::wxMessageBox(_("Failed to start terminal for debugger"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
            return false;
        }
    }

    // Launch local server if needed
    LLDBSettings settings;
    settings.Load();
    if(!settings.IsUsingRemoteProxy() && !m_connector.LaunchLocalDebugServer(settings.GetDebugserver())) {
        DoCleanup();
        return false;
    }

    return true;
}

void LLDBPlugin::OnDebugAttachToProcess(clDebugEvent& event)
{
    if(event.GetDebuggerName() != LLDB_DEBUGGER_NAME) {
        event.Skip();
        return;
    }

#ifdef __WXMSW__
    ::wxMessageBox(_("Attach to process with LLDB is not supported under Windows"), "CodeLite",
                   wxOK | wxCENTER | wxICON_WARNING);
    return;
#endif

    if(!DoInitializeDebugger(event, true, clDebuggerTerminalPOSIX::MakePidTitle(event.GetInt()))) return;

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
}

void LLDBPlugin::OnDebugDeleteAllBreakpoints(clDebugEvent& event)
{
    event.Skip();
    m_connector.DeleteAllBreakpoints();
}

void LLDBPlugin::OnDebugDisableAllBreakpoints(clDebugEvent& event) { event.Skip(); }

void LLDBPlugin::OnDebugEnableAllBreakpoints(clDebugEvent& event) { event.Skip(); }

void LLDBPlugin::OnDebugVOID(clDebugEvent& event) { CHECK_IS_LLDB_SESSION(); }
void LLDBPlugin::OnDebugNextInst(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    if(m_connector.IsCanInteract()) { m_connector.NextInstruction(); }
}

void LLDBPlugin::OnDebugShowCursor(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    if(m_connector.IsCanInteract()) { m_connector.ShowCurrentFileLine(); }
}

void LLDBPlugin::DestroyTooltip()
{
    if(m_tooltip) {
        m_tooltip->Destroy();
        m_tooltip = NULL;

        // Raise codelite back
        EventNotifier::Get()->TopFrame()->Raise();

        // If we destroyed the tooltip, set the focus back to the active editor
        IEditor* editor = m_mgr->GetActiveEditor();
        if(editor) { editor->SetActive(); }
    }
}

void LLDBPlugin::SetupPivotFolder(const LLDBConnectReturnObject& ret)
{
    if(!ret.IsPivotNeeded()) {
        m_connector.StartNetworkThread();
        return;
    }

    FolderMappingDlg dlg(NULL);
    LLDBPivot pivot;
    if(dlg.ShowModal() == wxID_OK) { m_connector.SetPivot(dlg.GetPivot()); }
    // Now that we got the pivot - start the network thread
    m_connector.StartNetworkThread();
}

void LLDBPlugin::OnDestroyTip(clCommandEvent& e)
{
    e.Skip();
    if(m_tooltip) {
        m_tooltip->Destroy();
        m_tooltip = NULL;
    }
}

void LLDBPlugin::OnLLDBLaunchSuccess(LLDBEvent& event)
{
    event.Skip();
    m_connector.SetCanInteract(true);
    m_connector.SetIsRunning(true);

    CL_DEBUG("CODELITE>> Applying breakpoints...");
    m_connector.ApplyBreakpoints();
    m_connector.Next();
}
