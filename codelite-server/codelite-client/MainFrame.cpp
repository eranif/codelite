#include "MainFrame.h"
#include <SocketAPI/clSocketClientAsync.h>
#include <wx/aboutdlg.h>

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
    , m_socket(new clSocketClientAsync(this))
    , m_connected(false)
{
    Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &MainFrame::OnConnection, this);
}

MainFrame::~MainFrame() { Unbind(wxEVT_ASYNC_SOCKET_CONNECTED, &MainFrame::OnConnection, this); }

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
    m_socket->Connect("tcp://127.0.0.1:5050");
    m_stc->AppendText("Connecting to tcp://127.0.0.1:5050\n");
    m_stc->ScrollToEnd();
}

void MainFrame::OnConnectUI(wxUpdateUIEvent& event) { event.Enable(!m_connected); }

void MainFrame::OnConnection(clCommandEvent& event)
{
    m_stc->AppendText("Connection estabilshed successfully\n");
    m_connected = true;
}
void MainFrame::OnDisconnect(wxCommandEvent& event) 
{
    if(m_connected) {
        m_socket->Disconnect();
        m_connected = false;
        m_stc->AppendText("Disconnected\n");
    }
}
void MainFrame::OnDisconnectUI(wxUpdateUIEvent& event) { event.Enable(m_connected); }
