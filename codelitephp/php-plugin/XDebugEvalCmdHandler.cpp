#include "XDebugEvalCmdHandler.h"

#include "XVariable.h"
#include "event_notifier.h"
#include "xdebugevent.h"
#include "xmlutils.h"

#include <wx/xml/xml.h>

XDebugEvalCmdHandler::XDebugEvalCmdHandler(const wxString& expression, int evalReason, XDebugManager* mgr, int transcationId)
    : XDebugCommandHandler(mgr, transcationId)
    , m_expression(expression)
    , m_evalReason(evalReason)
{
}

XDebugEvalCmdHandler::~XDebugEvalCmdHandler()
{
}

void XDebugEvalCmdHandler::Process(const wxXmlNode* response)
{
    // Search for the 'property' element
    wxXmlNode* xmlProp = XmlUtils::FindFirstByTagName(response, "property");
    if ( xmlProp ) {
        XVariable var( xmlProp, m_evalReason == kEvalForEvalPane );

        // Send an event
        XDebugEvent event(wxEVT_XDEBUG_EVAL_EXPRESSION);
        event.SetString( GetExpression() );
        event.SetEvaluated( var.value );
        event.SetEvalSucceeded(true);
        event.SetEvalReason(m_evalReason);
        EventNotifier::Get()->AddPendingEvent( event );

    } else {
        wxXmlNode* errorNode = XmlUtils::FindFirstByTagName(response, "error");
        if ( errorNode ) {
            wxXmlNode *message = XmlUtils::FindFirstByTagName(errorNode, "message");
            if ( message ) {
                XDebugEvent event(wxEVT_XDEBUG_EVAL_EXPRESSION);
                event.SetString( GetExpression() );
                event.SetEvalSucceeded(false);
                event.SetErrorString( message->GetNodeContent() );
                event.SetEvalReason(m_evalReason);
                EventNotifier::Get()->AddPendingEvent( event );
            }
        }
    }
}
