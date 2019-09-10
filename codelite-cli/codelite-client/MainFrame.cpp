#include "MainFrame.h"
#include <SocketAPI/clSocketClientAsync.h>
#include <wx/aboutdlg.h>
#include <wx/textdlg.h>

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
    , m_socket(nullptr)
    , m_connected(false)
{
    Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &MainFrame::OnConnection, this);
    Bind(wxEVT_ASYNC_SOCKET_INPUT, &MainFrame::OnSocketInput, this);
    Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &MainFrame::OnConnectionLost, this);
    Bind(wxEVT_ASYNC_SOCKET_ERROR, &MainFrame::OnConnectionLost, this);
}

MainFrame::~MainFrame()
{
    Unbind(wxEVT_ASYNC_SOCKET_CONNECTED, &MainFrame::OnConnection, this);
    Unbind(wxEVT_ASYNC_SOCKET_INPUT, &MainFrame::OnSocketInput, this);
    Unbind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &MainFrame::OnConnectionLost, this);
    Unbind(wxEVT_ASYNC_SOCKET_ERROR, &MainFrame::OnConnectionLost, this);
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetCopyright(_("My MainFrame"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("Short description goes here"));
    ::wxAboutBox(info);
}

void MainFrame::OnConnect(wxCommandEvent& event)
{
    if(m_socket) {
        wxDELETE(m_socket);
    }
    m_socket = new clSocketClientAsync(this, true);
    m_socket->Connect("tcp://127.0.0.1:5050");
    LogMessage("Connecting to tcp://127.0.0.1:5050");
}

void MainFrame::OnConnectUI(wxUpdateUIEvent& event) { event.Enable(!m_connected); }

void MainFrame::OnConnection(clCommandEvent& event)
{
    LogMessage("Connection estabilshed successfully");
    m_connected = true;
}

void MainFrame::OnDisconnect(wxCommandEvent& event)
{
    if(m_connected) {
        m_socket->Disconnect();
        wxDELETE(m_socket);
        m_connected = false;
        LogMessage("Disconnected");
    }
}
void MainFrame::OnDisconnectUI(wxUpdateUIEvent& event) { event.Enable(m_connected); }

void MainFrame::OnList(wxCommandEvent& event)
{
    wxString path = ::wxGetTextFromUser(_("Remote folder path:"), _("Path"), "/");
    if(path.IsEmpty()) return;

    clNetworkMessage message;
    message["command"] = "list";
    message["folder"] = path;
    message.SendMessage(m_socket);
}

void MainFrame::OnListUI(wxUpdateUIEvent& event) { event.Enable(m_connected); }

void MainFrame::LogMessage(const wxString& message)
{
    m_stc->AppendText(message + "\n");
    m_stc->ScrollToEnd();
}

void MainFrame::OnSocketInput(clCommandEvent& event) { LogMessage(event.GetString()); }

void MainFrame::OnConnectionLost(clCommandEvent& event)
{
    if(m_connected) {
        m_socket->Disconnect();
        wxDELETE(m_socket);
        m_connected = false;
        LogMessage("Connection lost, disconnected");
    }
}
