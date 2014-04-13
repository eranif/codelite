#include "lldbdebugger-plugin.h"
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "event_notifier.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "console_frame.h"
#include <wx/aui/framemanager.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>
#include "file_logger.h"
#include "environmentconfig.h"
#include "clcommandlineparser.h"
#include "dirsaver.h"
#include "bookmark_manager.h"
#include <wx/msgdlg.h>
#include "LLDBCallStack.h"
#include "LLDBSettings.h"
#include "bookmark_manager.h"
#include "LLDBBreakpointsPane.h"
#include "LLDBLocalsView.h"

static LLDBDebuggerPlugin* thePlugin = NULL;

#define DEBUGGER_NAME               "LLDB Debugger"
#define LLDB_CALLSTACK_PANE_NAME    "LLDB Callstack"
#define LLDB_BREAKPOINTS_PANE_NAME  "LLDB Breakpoints"
#define LLDB_LOCALS_PANE_NAME       "LLDB Locals"

#define CHECK_IS_LLDB_SESSION() if ( !m_connector.IsRunning() ) { event.Skip(); return; }

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new LLDBDebuggerPlugin(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("LLDBDebuggerPlugin"));
    info.SetDescription(wxT("LLDB Debugger for CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

LLDBDebuggerPlugin::LLDBDebuggerPlugin(IManager *manager)
    : IPlugin(manager)
    , m_callstack(NULL)
    , m_console(NULL)
    , m_breakpointsView(NULL)
    , m_localsView(NULL)
{
    m_longName = wxT("LLDB Debugger for CodeLite");
    m_shortName = wxT("LLDBDebuggerPlugin");

    m_connector.Bind(wxEVT_LLDB_STARTED,                &LLDBDebuggerPlugin::OnLLDBStarted, this);
    m_connector.Bind(wxEVT_LLDB_CRASHED,                &LLDBDebuggerPlugin::OnLLDBCrashed, this);
    m_connector.Bind(wxEVT_LLDB_EXITED,                 &LLDBDebuggerPlugin::OnLLDBExited,  this);
    m_connector.Bind(wxEVT_LLDB_STOPPED,                &LLDBDebuggerPlugin::OnLLDBStopped, this);
    m_connector.Bind(wxEVT_LLDB_RUNNING,                &LLDBDebuggerPlugin::OnLLDBRunning, this);
    m_connector.Bind(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY, &LLDBDebuggerPlugin::OnLLDBStoppedOnEntry, this);
    m_connector.Bind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL,&LLDBDebuggerPlugin::OnLLDBDeletedAllBreakpoints, this);
    m_connector.Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED,    &LLDBDebuggerPlugin::OnLLDBBreakpointsUpdated, this);
    
    // UI events
    EventNotifier::Get()->Connect(wxEVT_DBG_IS_PLUGIN_DEBUGGER, clDebugEventHandler(LLDBDebuggerPlugin::OnIsDebugger), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_START_OR_CONT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStart), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_NEXT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugNext), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_STEP_IN, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStepIn), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_STEP_OUT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStepOut), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_STOP, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStop), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_IS_RUNNING, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugIsRunning), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_CAN_INTERACT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugCanInteract), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, clDebugEventHandler(LLDBDebuggerPlugin::OnToggleBreakpoint), NULL, this);
}

void LLDBDebuggerPlugin::UnPlug()
{
    m_connector.StopDebugServer();
    DestroyUI();

    m_connector.Unbind(wxEVT_LLDB_STARTED,                &LLDBDebuggerPlugin::OnLLDBStarted, this);
    m_connector.Unbind(wxEVT_LLDB_CRASHED,                &LLDBDebuggerPlugin::OnLLDBCrashed, this);
    m_connector.Unbind(wxEVT_LLDB_EXITED,                 &LLDBDebuggerPlugin::OnLLDBExited,  this);
    m_connector.Unbind(wxEVT_LLDB_STOPPED,                &LLDBDebuggerPlugin::OnLLDBStopped, this);
    m_connector.Unbind(wxEVT_LLDB_RUNNING,                &LLDBDebuggerPlugin::OnLLDBRunning, this);
    m_connector.Unbind(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY, &LLDBDebuggerPlugin::OnLLDBStoppedOnEntry, this);
    m_connector.Unbind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL,&LLDBDebuggerPlugin::OnLLDBDeletedAllBreakpoints, this);
    m_connector.Unbind(wxEVT_LLDB_BREAKPOINTS_UPDATED,    &LLDBDebuggerPlugin::OnLLDBBreakpointsUpdated, this);
    
    // UI events
    EventNotifier::Get()->Disconnect(wxEVT_DBG_IS_PLUGIN_DEBUGGER, clDebugEventHandler(LLDBDebuggerPlugin::OnIsDebugger), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_START_OR_CONT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStart), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_NEXT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugNext), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_STOP, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStop), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_IS_RUNNING, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugIsRunning), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_CAN_INTERACT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugCanInteract), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_STEP_IN, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStepIn), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_STEP_OUT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStepOut), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, clDebugEventHandler(LLDBDebuggerPlugin::OnToggleBreakpoint), NULL, this);
}

LLDBDebuggerPlugin::~LLDBDebuggerPlugin()
{
}

clToolBar *LLDBDebuggerPlugin::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void LLDBDebuggerPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxUnusedVar(pluginsMenu);
}

void LLDBDebuggerPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void LLDBDebuggerPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}


void LLDBDebuggerPlugin::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    m_mgr->GetAllEditors( editors );
    IEditor::List_t::iterator iter = editors.begin();
    for(; iter != editors.end(); ++iter ) {
        (*iter)->GetSTC()->MarkerDeleteAll(smt_indicator);
    }
}

void LLDBDebuggerPlugin::SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno)
{
    stc->MarkerDeleteAll(smt_indicator);
    stc->MarkerAdd(lineno, smt_indicator);
    int caretPos = stc->PositionFromLine(lineno);
    stc->SetSelection(caretPos, caretPos);
    stc->SetCurrentPos( caretPos );
    stc->EnsureCaretVisible();
}

void LLDBDebuggerPlugin::OnDebugStart(clDebugEvent& event)
{
    if ( event.GetString() != DEBUGGER_NAME ) {
        event.Skip();
        return;
    }
    
    if ( !m_connector.IsRunning() ) {
        
        // Launch codelite-lldb
        m_connector.LaunchDebugServer();
        
        CL_DEBUG("LLDB: Initial working directory is restored to: " + ::wxGetCwd());
        {
            // Get the executable to debug
            wxString errMsg;
            ProjectPtr pProject = WorkspaceST::Get()->FindProjectByName(event.GetProjectName(), errMsg);
            if ( !pProject ) {
                ::wxMessageBox(wxString() << _("Could not locate project: ") << event.GetProjectName(), "LLDB Debugger", wxICON_ERROR|wxOK|wxCENTER);
                return;
            }

            DirSaver ds;
            ::wxSetWorkingDirectory ( pProject->GetFileName().GetPath() );

            BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf ( pProject->GetName(), wxEmptyString );
            if ( !bldConf ) {
                ::wxMessageBox(wxString() << _("Could not locate the requested buid configuration"), "LLDB Debugger", wxICON_ERROR|wxOK|wxCENTER);
                return;
            }

            // Determine the executable to debug, working directory and arguments
            EnvSetter env(NULL, NULL, pProject ? pProject->GetName() : wxString());
            wxString exepath = bldConf->GetCommand();
            wxString args;
            wxString wd;
            // Get the debugging arguments.
            if(bldConf->GetUseSeparateDebugArgs()) {
                args = bldConf->GetDebugArgs();
            } else {
                args = bldConf->GetCommandArguments();
            }

            wd      = ::ExpandVariables ( bldConf->GetWorkingDirectory(), pProject, m_mgr->GetActiveEditor() );
            exepath = ::ExpandVariables ( exepath, pProject, m_mgr->GetActiveEditor() );

            {
                DirSaver ds;
                ::wxSetWorkingDirectory(wd);
                wxFileName execToDebug( exepath );
                if ( execToDebug.IsRelative() ) {
                    execToDebug.MakeAbsolute();
                }

                CL_DEBUG("LLDB: Using executable : " + execToDebug.GetFullPath());
                CL_DEBUG("LLDB: Working directory: " + ::wxGetCwd());

                if ( m_connector.ConnectToDebugger(5) ) {
                    // display the terminal panel
                    wxString tty = ShowTerminal("LLDB Console Window");
                    
                    // Get list of breakpoints and add them ( we will apply them later on )
                    BreakpointInfo::Vec_t gdbBps;
                    m_mgr->GetAllBreakpoints(gdbBps);
                    
                    // remove all breakpoints from previous session
                    m_connector.DeleteAllBreakpoints();

                    // apply the serialized breakpoints
                    m_connector.AddBreakpoints( gdbBps );

                    LLDBCommand startCommand;
                    startCommand.SetExecutable( execToDebug.GetFullPath() );
                    startCommand.SetCommandArguments( args );
                    startCommand.SetRedirectTTY( tty );
                    startCommand.SetWorkingDirectory( wd );
                    m_connector.Start( startCommand );
                }
            }
        }
        CL_DEBUG("LLDB: Working directory is restored to: " + ::wxGetCwd());

    } else {
        CL_DEBUG("CODELITE>> continue...");
        m_connector.Continue();
    }
}

void LLDBDebuggerPlugin::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    
    // Stop the debugger ( do not notify about it, since we are in the handler...)
    m_connector.Cleanup();

    // Save current perspective before destroying the session
    SaveLLDBPerspective();

    // Restore the old perspective
    RestoreDefaultPerspective();
    DestroyUI();
    
    // Reset various state variables
    DoCleanup();
    
    CL_DEBUG("CODELITE>> LLDB exited");
    
    // Also notify codelite's event
    wxCommandEvent e2(wxEVT_DEBUG_ENDED);
    EventNotifier::Get()->AddPendingEvent( e2 );
}

void LLDBDebuggerPlugin::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    InitializeUI();
    LoadLLDBPerspective();
    
    // instruct the debugger to 'Run'
    m_connector.Run();

    CL_DEBUG("CODELITE>> LLDB started");
    wxCommandEvent e2(wxEVT_DEBUG_STARTED);
    EventNotifier::Get()->AddPendingEvent( e2 );
}

void LLDBDebuggerPlugin::OnLLDBStopped(LLDBEvent& event)
{
    event.Skip();
    CL_DEBUG(wxString() << "CODELITE>> LLDB stopped at " << event.GetFileName() << ":" << event.GetLinenumber() );
    m_connector.SetCanInteract(true);
    
    if ( event.GetInterruptReason() == kInterruptReasonNone ) {
        // Mark the debugger line / file
        IEditor *editor = m_mgr->FindEditor( event.GetFileName() );
        if ( !editor && wxFileName::Exists(event.GetFileName()) ) {
            // Try to open the editor
            if ( m_mgr->OpenFile(event.GetFileName(), "", event.GetLinenumber()-1) ) {
                editor = m_mgr->GetActiveEditor();
            }
        }
        
        if ( editor ) {
            // select it first
            if ( editor != m_mgr->GetActiveEditor() ) {
                m_mgr->SelectPage( editor->GetSTC() );
                
            } else {
                // just make sure that the page has the focus
                editor->SetActive();
            }
            
            // clear the markers
            ClearDebuggerMarker();
            SetDebuggerMarker(editor->GetSTC(), event.GetLinenumber() -1 );
        }
        
        // request for local variables
        m_connector.RequestLocals();
        
    } else if ( event.GetInterruptReason() == kInterruptReasonApplyBreakpoints ) {
        CL_DEBUG("Applying breakpoints and continue...");
        m_connector.ApplyBreakpoints();
        m_connector.Continue();

    } else if ( event.GetInterruptReason() == kInterruptReasonDeleteAllBreakpoints) {
        CL_DEBUG("Deleting all breakpoints");
        m_connector.DeleteAllBreakpoints();
        m_connector.Continue();
        
    } else if ( event.GetInterruptReason() == kInterruptReasonDeleteBreakpoint ) {
        CL_DEBUG("Deleting all pending deletion breakpoints");
        m_connector.DeleteBreakpoints();
        m_connector.Continue();
    }
}

void LLDBDebuggerPlugin::OnLLDBStoppedOnEntry(LLDBEvent& event)
{
    event.Skip();
    m_connector.SetCanInteract(true);
    m_connector.SetIsRunning(true);
    
    CL_DEBUG("CODELITE>> Applying breakpoints...");
    m_connector.ApplyBreakpoints();
    CL_DEBUG("CODELITE>> continue...");
    m_connector.Continue();
}

wxString LLDBDebuggerPlugin::ShowTerminal(const wxString &title)
{
    wxString consoleTitle = title;
    wxString ttyString;

    // Create a new TTY Console and place it in the AUI
    // Note that 'm_console' will call to Destroy() when the debug session
    // is terminated. In other words: don't worry about memory leaks here
    m_console = new ConsoleFrame(EventNotifier::Get()->TopFrame(), m_mgr);
    ttyString = m_console->StartTTY();
    consoleTitle << " (" << ttyString << ")";

    wxAuiPaneInfo paneInfo;
    wxAuiManager* dockManager = m_mgr->GetDockingManager();
    paneInfo.Name(wxT("LLDB Console")).Caption(consoleTitle).Dockable().FloatingSize(300, 200).CloseButton(false);
    dockManager->AddPane(m_console, paneInfo);

    // Re-set the title (it might be modified by 'LoadPerspective'
    wxAuiPaneInfo& pi = dockManager->GetPane(wxT("LLDB Console"));
    if(pi.IsOk()) {
        pi.Caption(consoleTitle);
    }

    wxAuiPaneInfo &info = dockManager->GetPane(wxT("LLDB Console"));
    if(info.IsShown() == false) {
        info.Show();
    }
    dockManager->Update();
    CL_DEBUG("CODELITE>> Using TTY: " + ttyString );
    return ttyString;
}

void LLDBDebuggerPlugin::OnDebugNext(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    CL_DEBUG("LLDB    >> Next");
    m_connector.Next();
}

void LLDBDebuggerPlugin::OnDebugStop(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    CL_DEBUG("LLDB    >> Stop");
    m_connector.Stop();
}

void LLDBDebuggerPlugin::OnDebugIsRunning(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    event.SetAnswer( m_connector.IsRunning() );
}

void LLDBDebuggerPlugin::OnDebugCanInteract(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    event.SetAnswer( m_connector.IsCanInteract() );
}

void LLDBDebuggerPlugin::OnDebugStepIn(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    m_connector.StepIn();
}

void LLDBDebuggerPlugin::OnDebugStepOut(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    m_connector.StepOut();
}

void LLDBDebuggerPlugin::OnIsDebugger(clDebugEvent& event)
{
    event.Skip();
    // register us as a debugger
    event.GetStrings().Add(DEBUGGER_NAME);
}

void LLDBDebuggerPlugin::LoadLLDBPerspective()
{
    // store the previous perspective before we continue
    m_defaultPerspective = m_mgr->GetDockingManager()->SavePerspective();

    LLDBSettings settings;
    wxString perspective = settings.Load().LoadPerspective();
    if ( !perspective.IsEmpty() ) {
        m_mgr->GetDockingManager()->LoadPerspective( perspective, false );
    }

    // Make sure that all the panes are visible
    ShowLLDBPane("LLDB Console");
    ShowLLDBPane(LLDB_CALLSTACK_PANE_NAME);
    ShowLLDBPane(LLDB_BREAKPOINTS_PANE_NAME);
    ShowLLDBPane(LLDB_LOCALS_PANE_NAME);
    m_mgr->GetDockingManager()->Update();
}

void LLDBDebuggerPlugin::SaveLLDBPerspective()
{
    LLDBSettings settings;
    settings.Load();
    settings.SavePerspective( m_mgr->GetDockingManager()->SavePerspective() );
    settings.Save();
}

void LLDBDebuggerPlugin::ShowLLDBPane(const wxString& paneName, bool show)
{
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(paneName);
    if ( pi.IsOk() ) {
        if ( show ) {
            if ( !pi.IsShown() ) {
                pi.Show();
            }
        } else {
            if ( pi.IsShown() ) {
                pi.Hide();
            }
        }
    }
}

void LLDBDebuggerPlugin::RestoreDefaultPerspective()
{
    if ( !m_defaultPerspective.IsEmpty() ) {
        m_mgr->GetDockingManager()->LoadPerspective( m_defaultPerspective, true );
        m_defaultPerspective.Clear();
    }
}

void LLDBDebuggerPlugin::DestroyUI()
{
    // Destroy the callstack window
    if ( m_callstack ) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(LLDB_CALLSTACK_PANE_NAME);
        if ( pi.IsOk() ) {
            m_mgr->GetDockingManager()->DetachPane(m_callstack);
        }
        m_callstack->Destroy();
        m_callstack = NULL;
    }
    if ( m_breakpointsView ) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(LLDB_BREAKPOINTS_PANE_NAME);
        if ( pi.IsOk() ) {
            m_mgr->GetDockingManager()->DetachPane(m_breakpointsView);
        }
        m_breakpointsView->Destroy();
        m_breakpointsView = NULL;
    }
    if ( m_localsView ) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(LLDB_LOCALS_PANE_NAME);
        if ( pi.IsOk() ) {
            m_mgr->GetDockingManager()->DetachPane(m_localsView);
        }
        m_localsView->Destroy();
        m_localsView = NULL;
    }
}

void LLDBDebuggerPlugin::InitializeUI()
{
    if ( !m_callstack ) {
        m_callstack = new LLDBCallStackPane(EventNotifier::Get()->TopFrame(), &m_connector);
        m_mgr->GetDockingManager()->AddPane(m_callstack, wxAuiPaneInfo().Bottom().Position(0).CloseButton().Caption("Callstack").Name(LLDB_CALLSTACK_PANE_NAME));
    }
    
    if ( !m_breakpointsView ) {
        m_breakpointsView = new LLDBBreakpointsPane(EventNotifier::Get()->TopFrame(), this);
        m_mgr->GetDockingManager()->AddPane(m_breakpointsView, wxAuiPaneInfo().Bottom().Position(1).CloseButton().Caption("Breakpoints").Name(LLDB_BREAKPOINTS_PANE_NAME));
    }
    
    if ( !m_localsView ) {
        m_localsView = new LLDBLocalsView(EventNotifier::Get()->TopFrame(), this);
        m_mgr->GetDockingManager()->AddPane(m_localsView, wxAuiPaneInfo().Top().Position(0).CloseButton().Caption("Locals").Name(LLDB_LOCALS_PANE_NAME));
    }
}

void LLDBDebuggerPlugin::OnLLDBRunning(LLDBEvent& event)
{
    event.Skip();
    m_connector.SetCanInteract(false);
    
    // When the IDE loses the focus - clear the debugger marker
    ClearDebuggerMarker();

    // set the focus to the terminal (incase a user input is requested)
    m_console->SetFocus();
}

void LLDBDebuggerPlugin::OnToggleBreakpoint(clDebugEvent& event)
{
    // Call Skip() here since we want codelite to manage the breakpoint as well ( in term of serilization in the session file )
    CHECK_IS_LLDB_SESSION();
    
    // check to see if we are removing a breakpoint or adding one
    LLDBBreakpoint::Ptr_t bp( new LLDBBreakpoint(event.GetFileName(), event.GetInt() ) );
    IEditor * editor = m_mgr->GetActiveEditor();
    
    if ( editor ) {
        // get the marker type set on the line
        int markerType = editor->GetSTC()->MarkerGet(bp->GetLineNumber()-1);
        for (size_t type=smt_FIRST_BP_TYPE; type <= smt_LAST_BP_TYPE; ++type) {
            int markerMask = ( 1 << type );
            if ( markerType & markerMask ) {
                // removing a breakpoint. "DeleteBreakpoint" will handle the interactive/non-interactive mode 
                // of the debugger
                m_connector.MarkBreakpointForDeletion( bp );
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

void LLDBDebuggerPlugin::DoCleanup()
{
    ClearDebuggerMarker();
    m_console = NULL;
}

void LLDBDebuggerPlugin::OnLLDBDeletedAllBreakpoints(LLDBEvent& event)
{
    event.Skip();
    m_mgr->DeleteAllBreakpoints();
}

void LLDBDebuggerPlugin::OnLLDBBreakpointsUpdated(LLDBEvent& event)
{
    event.Skip();
    // update the ui (mainly editors)
    // this is done by replacing the breakpoints list with a new one (the updated one we take from LLDB)
    m_mgr->SetBreakpoints( LLDBBreakpoint::ToBreakpointInfoVector( event.GetBreakpoints() ) );
}

void LLDBDebuggerPlugin::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    if ( m_connector.IsRunning() ) {
        m_connector.Stop();
    }
    m_connector.Cleanup();
}

void LLDBDebuggerPlugin::OnWorkspaceLoaded(wxCommandEvent& event)
{
    event.Skip();
}

void LLDBDebuggerPlugin::OnLLDBCrashed(LLDBEvent& event)
{
    event.Skip();
    ::wxMessageBox(_("LLDB crashed! Terminating debug session"), "CodeLite", wxOK|wxICON_ERROR|wxCENTER);
    OnLLDBExited( event );
}
