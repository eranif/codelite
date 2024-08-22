#include "XDebugContextGetCmdHandler.h"

#include "XDebugManager.h"
#include "event_notifier.h"
#include "xdebugevent.h"

#include <wx/xml/xml.h>

XDebugContextGetCmdHandler::XDebugContextGetCmdHandler(XDebugManager* mgr, int transcationId, int requestedStack)
    : XDebugCommandHandler(mgr, transcationId)
    , m_requestedStack(requestedStack)
{
}

XDebugContextGetCmdHandler::~XDebugContextGetCmdHandler()
{
}

void XDebugContextGetCmdHandler::Process(const wxXmlNode* response)
{
    XVariable::List_t variables;
    // got the reply from XDebug parse and display the locals
    wxXmlNode* child = response->GetChildren();
    while ( child ) {
        if ( child->GetName() == "property" ) {
            XVariable var(child);
            variables.push_back( var );
        }
        child = child->GetNext();
    }
    
    XDebugEvent event(wxEVT_XDEBUG_LOCALS_UPDATED);
    event.SetVariables( variables );
    EventNotifier::Get()->AddPendingEvent( event );
}
