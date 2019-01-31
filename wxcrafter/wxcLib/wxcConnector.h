#ifndef WXCCONNECTOR_H
#define WXCCONNECTOR_H

#include <wx/event.h>
#include <wx/process.h>
#include "wxcLib/clSocketBase.h"
#include "wxcLib/wxcNetworkCommand.h"


class wxcNetworkReplyThread;
class wxcConnector : public wxEvtHandler
{
    clSocketBase::Ptr_t     m_socket;
    wxProcess*              m_wxcrafterProcess;
    wxString                m_wxcrafterExecCommand;
    wxcNetworkReplyThread*  m_networkThread;
    
protected:
    DECLARE_EVENT_TABLE()
    void OnProcessTerminated(wxProcessEvent& event);
    void OnFilesGenerated(wxCommandEvent& event);
    
private:
    void DoLaunchAndConnect() ;
    
public:
    wxcConnector();
    virtual ~wxcConnector();
    
    /**
     * @brief return true if this instance is connected to wxCrafter
     */
    bool IsConnected() const;
    /**
     * @brief shutdown wxCrafter and perform cleanup
     */
    void Shutdown();
    
    /**
     * @brief launch wxCrafter in server-mode
     * @param wxcPath wxCrafter executable path
     * @throw clSocketException
     */
    void LaunchAndConnect(const wxString &wxcPath) ;

    /**
     * @brief send a network command to wxCrafter
     * @param command command to send
     * @throw clSocketException
     */
    void SendCommand(const wxcNetworkCommand& command) ;
    
    /**
     * @brief load a wxCrafter file into wxCrafter and show the designer (if not shown)
     * @param filename full path the file to load
     */
    void LoadFile( const wxFileName& filename ) ;
    
    /**
     * @brief generate code for the wxcp file
     * @param wxcpFileName
     */
    void GenerateCode( const wxFileName& wxcpFileName) ;
    
    /**
     * @brief create new form
     * @param wxcpFileName
     * @param formType
     */
    void NewForm(const wxFileName& wxcpFile, wxCrafter::eCommandType formType);
};

#endif // WXCCONNECTOR_H
