#include "XDebugStopCmdHandler.h"

#include "XDebugManager.h"
#include "xdebugevent.h"

#include <event_notifier.h>
#include <file_logger.h>
#include <wx/socket.h>
#include <wx/xml/xml.h>

XDebugStopCmdHandler::XDebugStopCmdHandler(XDebugManager* mgr, int transcationId)
    : XDebugCommandHandler(mgr, transcationId)
{
}

XDebugStopCmdHandler::~XDebugStopCmdHandler() {}

void XDebugStopCmdHandler::Process(const wxXmlNode* response)
{
    clDEBUG() << "CodeLite: Stop command completed." << endl;
    wxString status = response->GetAttribute("status");
    if(status == "stopping") {
        clDEBUG() << "CodeLite: xdebug entered status 'stopping'" << endl;

        // Notify about control
        XDebugEvent focusEvent(wxEVT_XDEBUG_STOPPED);
        EventNotifier::Get()->AddPendingEvent(focusEvent);

    } else if(status == "stopped") {
        clDEBUG() << "CodeLite: xdebug entered status 'stopped'" << endl;
        m_mgr->SendStopCommand();

    } else {
        // default
        m_mgr->CloseDebugSession();
    }
}
