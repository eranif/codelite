#include "MainDialog.h"
#include "LLDBProtocol/LLDBConnector.h"

MainDialog::MainDialog(wxWindow* parent)
    : MainDialogBaseClass(parent)
{
    m_connector.ConnectToDebugger();
    m_connector.Bind(wxEVT_LLDB_STARTED, &MainDialog::OnLLDBStarted, this);
    m_connector.Bind(wxEVT_LLDB_EXITED,  &MainDialog::OnLLDBStarted, this);
    m_connector.Bind(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY,  &MainDialog::OnLLDBStoppedOnFirstEntry, this);
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
}
