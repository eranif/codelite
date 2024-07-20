#include "XDebugUnknownCommand.h"

#include "event_notifier.h"
#include "xdebugevent.h"

#include <wx/sstream.h>
#include <wx/xml/xml.h>

XDebugUnknownCommand::XDebugUnknownCommand(XDebugManager* mgr, int transcationId)
    : XDebugCommandHandler(mgr, transcationId)
{
}

XDebugUnknownCommand::~XDebugUnknownCommand()
{
}

void XDebugUnknownCommand::Process(const wxXmlNode* response)
{
    wxXmlDocument doc;
    doc.SetRoot( const_cast<wxXmlNode*>(response) );
    
    wxString asString;
    wxStringOutputStream sos( &asString );
    doc.Save( sos );
    doc.DetachRoot();
    
    XDebugEvent event(wxEVT_XDEBUG_UNKNOWN_RESPONSE);
    event.SetEvaluated( asString );
    EventNotifier::Get()->AddPendingEvent( event );
}
