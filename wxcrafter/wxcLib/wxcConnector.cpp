#include "wxcConnector.h"
#include <wx/utils.h>
#include "wxcLib/clSocketClient.h"
#include "wxcLib/wxcLibDefines.h"
#include <wx/socket.h>
#include "wxcLib/wxcNetworkReplyThread.h"
#include "wxcLib/wxcReplyEventData.h"
#include <wx/msgdlg.h>

const int ID_WXC_PROCESS = ::wxNewId();

BEGIN_EVENT_TABLE(wxcConnector, wxEvtHandler)
    EVT_END_PROCESS(ID_WXC_PROCESS, wxcConnector::OnProcessTerminated)
END_EVENT_TABLE()

wxcConnector::wxcConnector()
    : m_wxcrafterProcess(NULL)
    , m_networkThread(NULL)
{

#ifdef __WXMSW__
    static bool bInitialized = false;
    if ( !bInitialized ) {
        WORD wVersionRequested;
        WSADATA wsaData;
        // Using MAKEWORD macro, Winsock version request 2.2
        wVersionRequested = MAKEWORD(2, 2);
        WSAStartup(wVersionRequested, &wsaData);
        bInitialized = true;
    }
#endif

    Connect(wxcEVT_NET_REPLY_FILES_GENERATED, wxCommandEventHandler(wxcConnector::OnFilesGenerated), NULL, this);
}

wxcConnector::~wxcConnector()
{
    Shutdown();
}

void wxcConnector::DoLaunchAndConnect() 
{
    m_wxcrafterProcess = new wxProcess(this, ID_WXC_PROCESS);
    long pid = ::wxExecute(m_wxcrafterExecCommand, wxEXEC_ASYNC, m_wxcrafterProcess);
    if ( pid > 0 ) {
        clSocketClient* client = new clSocketClient();
        m_socket.reset( client );

        // wait for the process to launch and connect to it
        // we wait up to 5 seconds
        int nRetries = 100;
        do {
            if ( client->ConnectRemote(wxT("127.0.0.1"), WXC_PORT_NUMBER_INT)) {
                break;
            }

            --nRetries;
            wxThread::Sleep(50);
        } while ( nRetries > 0 );

        if ( nRetries <= 0 ) {
            wxString message;
            message << _("Could not connect to wxCrafter in a timely manner");
            Shutdown();

            throw clSocketException( message );
        }

        // we are connected, use the same descriptor to create a listener thread
        m_socket->SetCloseOnExit( false );

        m_networkThread = new wxcNetworkReplyThread(this, m_socket->GetSocket());
        m_networkThread->Start();

    } else {
        wxString message;
        message << _("Could not start wxCrafter at: ") << m_wxcrafterExecCommand;
        throw clSocketException( message );
    }
}

void wxcConnector::LaunchAndConnect(const wxString& wxcPath) 
{
    wxString command;
    command << wxcPath;
    if ( command.Contains(wxT(" ")) ) {
        command.Prepend(wxT("\"")).Append(wxT("\""));
    }
    command << wxT(" -s");
    m_wxcrafterExecCommand = command;
    DoLaunchAndConnect();
}

void wxcConnector::SendCommand(const wxcNetworkCommand& command) 
{
    if ( !m_socket ) {
        throw clSocketException(wxT("socket not connected!"));
    }

    // Serialize the command
    wxString serializedCommand = command.ToJSON().format();
    m_socket->WriteMessage( serializedCommand );
}

void wxcConnector::OnProcessTerminated(wxProcessEvent& event)
{
    wxUnusedVar( event );
    
    // Call shutdown to perform cleanup
    Shutdown();
}

void wxcConnector::Shutdown()
{
    m_socket.reset( NULL );

    // The network thread should be destroyed after the socket
    if ( m_networkThread ) {
        m_networkThread->Stop();
    }
    wxDELETE( m_networkThread );
    if ( m_wxcrafterProcess ) {

        // don't send us any more events
        m_wxcrafterProcess->Detach();

        // kill wxCrafter
        wxProcess::Kill( m_wxcrafterProcess->GetPid() );

    }
    m_wxcrafterProcess = NULL;
}

void wxcConnector::LoadFile(const wxFileName& filename) 
{
    wxcNetworkCommand command;
    command.SetCommandType( wxCrafter::kCommandTypeLoadFile );
    command.SetFilename( filename.GetFullPath() );
    SendCommand( command );
}

void wxcConnector::OnFilesGenerated(wxCommandEvent& event)
{
    event.Skip();

    wxcReplyEventData *data = dynamic_cast<wxcReplyEventData*>( event.GetClientObject() );
    if ( data ) {
        wxString message;
        const wxcReplyEventData::StringVec_t &files = data->GetFiles();
        for(size_t i=0; i<files.size(); ++i) {
            message << files.at(i) << wxT("\n");
        }
        ::wxMessageBox(message, wxT("wxCrafter Connector"));
        wxDELETE( data );
    }
}

void wxcConnector::GenerateCode(const wxFileName& wxcpFileName) 
{
    wxcNetworkCommand command;
    command.SetCommandType( wxCrafter::kCommandTypeGenerateCode );
    command.SetFilename( wxcpFileName.GetFullPath() );
    SendCommand( command );
}

bool wxcConnector::IsConnected() const
{
    return m_wxcrafterProcess && m_networkThread;
}

void wxcConnector::NewForm(const wxFileName& wxcpFile, wxCrafter::eCommandType formType)
{
    wxcNetworkCommand command;
    command.SetCommandType( formType );
    command.SetFilename( wxcpFile.GetFullPath() );
    SendCommand( command );
}
