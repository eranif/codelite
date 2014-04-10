#include "MainDialog.h"
#include "LLDBProtocol/LLDBConnector.h"

MainDialog::MainDialog(wxWindow* parent)
    : MainDialogBaseClass(parent)
{
    m_connector.ConnectToDebugger();
    m_connector.Bind(wxEVT_LLDB_STARTED, &MainDialog::OnLLDBStarted, this);
    m_connector.Bind(wxEVT_LLDB_RUNNING, &MainDialog::OnLLDBRunning, this);
    m_connector.Bind(wxEVT_LLDB_EXITED,  &MainDialog::OnLLDBExited, this);
    m_connector.Bind(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY,  &MainDialog::OnLLDBStoppedOnFirstEntry, this);
    m_connector.Bind(wxEVT_LLDB_STOPPED,  &MainDialog::OnLLDBStopped, this);
    m_connector.Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED,  &MainDialog::OnLLDBBreakpointsUpdated, this);
}

MainDialog::~MainDialog()
{
}

void MainDialog::OnStart(wxCommandEvent& event)
{
    LLDBCommand command;
    command.SetCommandType( kCommandStart );
    command.SetExecutable( m_filePickerExe->GetPath() );
    m_connector.SendCommand( command );
    LogCommand( command );
}

void MainDialog::OnStop(wxCommandEvent& event)
{
    m_connector.Stop();
}

void MainDialog::LogCommand(const LLDBCommand& command)
{
    wxString text = command.ToJSON().format();
    m_textCtrlLog->AppendText( text + "\n" );
}

void MainDialog::OnLLDBExited(LLDBEvent& e)
{
    e.Skip();
    m_textCtrlLog->AppendText(wxString() << "LLDB Exited\n");
}

void MainDialog::OnLLDBStarted(LLDBEvent& e)
{
    e.Skip();
    m_textCtrlLog->AppendText(wxString() << "LLDB Started successfully\n");
    
    LLDBCommand command;
    command.SetCommandType( kCommandRun );
    command.SetCommandArguments( "" );
    m_connector.SendCommand( command );
    m_textCtrlLog->AppendText("Running...\n");
}

void MainDialog::OnLLDBStoppedOnFirstEntry(LLDBEvent& e)
{
    e.Skip();
    m_textCtrlLog->AppendText(wxString() << "LLDB stopped on first entry\n");
    
    // place all breakpoints and move on
    m_textCtrlLog->AppendText("Setting breakpoint at main...\n");
    m_connector.AddBreakpoint("main");
    m_connector.ApplyBreakpoints();
    m_textCtrlLog->AppendText("Continue...\n");
    m_connector.Continue();
}

void MainDialog::OnLLDBBreakpointsUpdated(LLDBEvent& e)
{
    e.Skip();
    
    wxString msg;
    const LLDBBreakpoint::Vec_t &bps = e.GetBreakpoints();
    for(size_t i=0; i<bps.size(); ++i) {
        msg << bps.at(i)->ToString() << "\n";
    }
    
    m_textCtrlLog->AppendText("Breakpoints updated:\n");
    m_textCtrlLog->AppendText( msg );
}

void MainDialog::OnLLDBRunning(LLDBEvent& e)
{
    e.Skip();
    m_textCtrlLog->AppendText("Debuggee process state is Running...\n");
}

void MainDialog::OnLLDBStopped(LLDBEvent& e)
{
    e.Skip();
    
    wxString msg;
    msg << "Process stopped. " << e.GetFileName() << ":" << e.GetLinenumber();
    m_textCtrlLog->AppendText(msg + "\n");
    m_connector.Continue();
}
