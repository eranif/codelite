#include "XDebugPropertyGetHandler.h"

#include "XVariable.h"
#include "event_notifier.h"
#include "macros.h"
#include "xdebugevent.h"

#include <wx/xml/xml.h>

XDebugPropertyGetHandler::XDebugPropertyGetHandler(XDebugManager* mgr, int transactionId, const wxString& property)
    : XDebugCommandHandler(mgr, transactionId)
    , m_property(property)
{
}

void XDebugPropertyGetHandler::Process(const wxXmlNode* response)
{
    CHECK_PTR_RET(response);

    // got the reply from XDebug parse and display the locals
    XVariable::List_t variables;
    wxXmlNode* child = response->GetChildren();
    if (child) {
        if (child->GetName() == "property") {
            XVariable var(child);
            variables.push_back(var);
        }
        child = child->GetNext();
    }

    XDebugEvent event(wxEVT_XDEBUG_PROPERTY_GET);
    event.SetVariables(variables);
    event.SetEvaluated(m_property);
    EventNotifier::Get()->AddPendingEvent(event);
}
