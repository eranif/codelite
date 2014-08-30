#include "XDebugStopCmdHandler.h"
#include <wx/socket.h>
#include <file_logger.h>
#include <wx/xml/xml.h>
#include "XDebugManager.h"
#include <event_notifier.h>
#include "xdebugevent.h"

XDebugStopCmdHandler::XDebugStopCmdHandler(XDebugManager* mgr, int transcationId)
    : XDebugCommandHandler(mgr, transcationId)
{
}

XDebugStopCmdHandler::~XDebugStopCmdHandler()
{
}

void XDebugStopCmdHandler::Process(const wxXmlNode* response)
{
    CL_DEBUG("CodeLite: Stop command completed.");
    wxString status = response->GetAttribute("status");
    if ( status == "stopping" ) {
        CL_DEBUG("CodeLite: xdebug entered status 'stopping'");
        
        // Notify about control
        XDebugEvent focusEvent(wxEVT_XDEBUG_STOPPED);
        EventNotifier::Get()->AddPendingEvent( focusEvent );

    } else if ( status == "stopped" ) {
        CL_DEBUG("CodeLite: xdebug entered status 'stopped'");
        m_mgr->SendStopCommand();
        
    } else {
        // default
        m_mgr->CloseDebugSession();
    }
}
