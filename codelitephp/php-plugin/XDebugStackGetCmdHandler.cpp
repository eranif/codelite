#include "XDebugStackGetCmdHandler.h"
#include <wx/socket.h>
#include <wx/xml/xml.h>
#include <xmlutils.h>
#include <cl_command_event.h>
#include <event_notifier.h>
#include "XDebugManager.h"
#include "xdebugevent.h"
XDebugStackGetCmdHandler::XDebugStackGetCmdHandler(XDebugManager* mgr, int transcationId, int requestedStack)
    : XDebugCommandHandler(mgr, transcationId)
    , m_requestedStack(requestedStack)
{
}

XDebugStackGetCmdHandler::~XDebugStackGetCmdHandler()
{
}

void XDebugStackGetCmdHandler::Process(const wxXmlNode* response)
{
    wxArrayString stackTrace;
    wxXmlNode *child = response->GetChildren();
    while ( child ) {
        if ( child->GetName() == "stack" ) {
            wxString level    = child->GetAttribute("level");
            wxString where    = child->GetAttribute("where");
            wxString filename = child->GetAttribute("filename");
            int line_number = XmlUtils::ReadLong(child, "lineno");
            
            // Use pipe to separate the attributes
            wxString stackEntry;
            stackEntry << level << "|" << where << "|" << filename << "|" << line_number;
            stackTrace.Add( stackEntry );
        }
        child = child->GetNext();
    }
    
    XDebugEvent eventStack(wxEVT_XDEBUG_STACK_TRACE);
    eventStack.SetStrings( stackTrace );
    eventStack.SetInt(m_requestedStack);
    EventNotifier::Get()->AddPendingEvent( eventStack );
}
