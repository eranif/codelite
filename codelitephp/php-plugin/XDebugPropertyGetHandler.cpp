#include "XDebugPropertyGetHandler.h"
#include "macros.h"
#include "XVariable.h"
#include <wx/xml/xml.h>
#include "xdebugevent.h"
#include "event_notifier.h"

XDebugPropertyGetHandler::XDebugPropertyGetHandler(XDebugManager* mgr, int transcationId, const wxString &property)
    : XDebugCommandHandler(mgr, transcationId)
    , m_property(property)
{
}

XDebugPropertyGetHandler::~XDebugPropertyGetHandler()
{
}

void XDebugPropertyGetHandler::Process(const wxXmlNode* response)
{
    CHECK_PTR_RET(response);
    
    // got the reply from XDebug parse and display the locals
    XVariable::List_t variables;
    wxXmlNode* child = response->GetChildren();
    if ( child ) {
        if ( child->GetName() == "property" ) {
            XVariable var(child);
            variables.push_back( var );
        }
        child = child->GetNext();
    }
    
    XDebugEvent event(wxEVT_XDEBUG_PROPERTY_GET);
    event.SetVariables( variables );
    event.SetEvaluated( m_property );
    EventNotifier::Get()->AddPendingEvent( event );
}
