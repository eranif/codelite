#include "lldbdebugger-plugin.h"
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "event_notifier.h"
#include "LLDBEvent.h"
#include "console_frame.h"
#include <wx/aui/framemanager.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>

static LLDBDebuggerPlugin* thePlugin = NULL;

#define CHECK_IS_LLDB_SESSION() if ( !m_isRunning ) { event.Skip(); return; }

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
    , m_isRunning(false)
{
    LLDBDebugger::Initialize();
    m_longName = wxT("LLDB Debugger for CodeLite");
    m_shortName = wxT("LLDBDebuggerPlugin");
    
    m_debugger.Bind(wxEVT_LLDB_STARTED, &LLDBDebuggerPlugin::OnLLDBStarted, this);
    m_debugger.Bind(wxEVT_LLDB_EXITED,  &LLDBDebuggerPlugin::OnLLDBExited,  this);
    m_debugger.Bind(wxEVT_LLDB_STOPPED, &LLDBDebuggerPlugin::OnLLDBStopped, this);
    
    // UI events
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_START_OR_CONT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStart), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_NEXT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugNext), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_STOP, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStop), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DBG_IS_RUNNING, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugIsRunning), NULL, this);
}

LLDBDebuggerPlugin::~LLDBDebuggerPlugin()
{
    LLDBDebugger::Terminate();
}

clToolBar *LLDBDebuggerPlugin::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void LLDBDebuggerPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
}

void LLDBDebuggerPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
}

void LLDBDebuggerPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
}

void LLDBDebuggerPlugin::UnPlug()
{
    m_debugger.Unbind(wxEVT_LLDB_STARTED, &LLDBDebuggerPlugin::OnLLDBStarted, this);
    m_debugger.Unbind(wxEVT_LLDB_EXITED,  &LLDBDebuggerPlugin::OnLLDBExited,  this);
    m_debugger.Unbind(wxEVT_LLDB_STOPPED, &LLDBDebuggerPlugin::OnLLDBStopped, this);
    
    // UI events
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_START_OR_CONT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStart), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_NEXT, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugNext), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_STOP, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugStop), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DBG_IS_RUNNING, clDebugEventHandler(LLDBDebuggerPlugin::OnDebugIsRunning), NULL, this);
}

void LLDBDebuggerPlugin::OnDebugStart(clDebugEvent& event)
{
    if ( ::PromptForYesNoDialogWithCheckbox(_("Would you like to use LLDB debugger as your primary debugger?"), "UseLLDB") != wxID_YES ) {
        event.Skip();
        return;
    }
    
    // Get the executable to debug
    wxString errMsg;
    ProjectPtr pProject = WorkspaceST::Get()->FindProjectByName(event.GetProjectName(), errMsg);
    if ( !pProject ) {
        event.Skip();
        return;
    }
    
    wxSetWorkingDirectory ( pProject->GetFileName().GetPath() );
    BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf ( pProject->GetName(), wxEmptyString );
    if ( !bldConf ) {
        event.Skip();
        return;
    }
    
    // Show the terminal
    ShowTerminal("LLDB Console Window");
    if ( m_debugger.Start("/home/eran/devl/TestArea/TestHang/Debug/TestHang") ) {
        m_debugger.AddBreakpoint("main");
        m_debugger.ApplyBreakpoints();
        m_debugger.Run(wxArrayString(), wxArrayString(), ::wxGetCwd());
    }
}

void LLDBDebuggerPlugin::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    m_isRunning = false;
    
    // Also notify codelite's event
    wxCommandEvent e2(wxEVT_DEBUG_ENDED);
    EventNotifier::Get()->AddPendingEvent( e2 );
}

void LLDBDebuggerPlugin::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    m_isRunning = true;
    
    wxCommandEvent e2(wxEVT_DEBUG_STARTED);
    EventNotifier::Get()->AddPendingEvent( e2 );
}

void LLDBDebuggerPlugin::OnLLDBStopped(LLDBEvent& event)
{
    event.Skip();
    wxFileName fn( event.GetFileName() );
    if ( fn.FileExists() ) {
        if ( m_mgr->OpenFile( fn.GetFullPath(), "", event.GetLinenumber() ) ) {
            IEditor* editor = m_mgr->GetActiveEditor();
            if ( editor ) {
                editor->GetSTC()->ScrollToLine( event.GetLinenumber() );
            }
        }
    }
}

void LLDBDebuggerPlugin::ShowTerminal(const wxString &title)
{
#ifndef __WXMSW__
    wxString consoleTitle = title;
    wxString ttyString;
    // Create a new TTY Console and place it in the AUI
    ConsoleFrame *console = new ConsoleFrame(EventNotifier::Get()->TopFrame(), m_mgr);
    ttyString = console->StartTTY();
    consoleTitle << " (" << ttyString << ")";
    
    wxAuiPaneInfo paneInfo;
    wxAuiManager* dockManager = m_mgr->GetDockingManager();
    paneInfo.Name(wxT("LLDB Console")).Caption(consoleTitle).Dockable().FloatingSize(300, 200).CloseButton(false);
    dockManager->AddPane(console, paneInfo);

    // Re-set the title (it might be modified by 'LoadPerspective'
    wxAuiPaneInfo& pi = dockManager->GetPane(wxT("LLDB Console"));
    if(pi.IsOk()) {
        pi.Caption(consoleTitle);
    }

    wxAuiPaneInfo &info = dockManager->GetPane(wxT("LLDB Console"));
    if(info.IsShown() == false) {
        info.Show();
        dockManager->Update();
    }
    m_debugger.SetTty( ttyString );
#endif
}

void LLDBDebuggerPlugin::OnDebugNext(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    m_debugger.StepOver();
}

void LLDBDebuggerPlugin::OnDebugStop(clDebugEvent& event)
{
    CHECK_IS_LLDB_SESSION();
    m_debugger.Stop();
}

void LLDBDebuggerPlugin::OnDebugIsRunning(clDebugEvent& event)
{
    event.SetAnswer( m_isRunning );
}
