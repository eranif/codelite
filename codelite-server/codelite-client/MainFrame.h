#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"
#include <cl_command_event.h>

class clSocketClientAsync;
class MainFrame : public MainFrameBaseClass
{
    clSocketClientAsync* m_socket;
    bool m_connected;
    
protected:
    void LogMessage(const wxString& message);
    bool IsConnected() const { return m_connected; }
    
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

protected:
    virtual void OnList(wxCommandEvent& event);
    virtual void OnListUI(wxUpdateUIEvent& event);
    virtual void OnDisconnect(wxCommandEvent& event);
    virtual void OnDisconnectUI(wxUpdateUIEvent& event);
    virtual void OnConnect(wxCommandEvent& event);
    virtual void OnConnectUI(wxUpdateUIEvent& event);
    // Socket events
    virtual void OnConnection(clCommandEvent& event);
    virtual void OnSocketInput(clCommandEvent& event);
    virtual void OnConnectionLost(clCommandEvent& event);
};
#endif // MAINFRAME_H
