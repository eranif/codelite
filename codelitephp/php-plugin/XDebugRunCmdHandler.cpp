#include "XDebugRunCmdHandler.h"
#include <wx/socket.h>
#include <wx/xml/xml.h>
#include <file_logger.h>
#include "XDebugManager.h"
#include <xmlutils.h>
#include <imanager.h>
#include "php.h" // PhpPlugin
#include <event_notifier.h>
#include "php_utils.h"
#include "xdebugevent.h"
#include <wx/msgdlg.h>

XDebugRunCmdHandler::XDebugRunCmdHandler(XDebugManager* mgr, int transcationId)
    : XDebugCommandHandler(mgr, transcationId)
{
}

XDebugRunCmdHandler::~XDebugRunCmdHandler()
{
}

void XDebugRunCmdHandler::Process(const wxXmlNode* response)
{
    // a reply to the "Run" command has arrived
    wxString status = response->GetAttribute("status");
    if ( status == "stopping" ) {
        CL_DEBUG("CodeLite >>> xdebug entered status 'stopping'");
        m_mgr->SendStopCommand();
        
    } else if ( status == "break" ) {
        // Break point was hit
        CL_DEBUG("CodeLite >>> Breakpoint was hit");
        wxXmlNode* msg = XmlUtils::FindFirstByTagName(response, "xdebug:message");
        if ( msg ) {
            wxString filename = msg->GetAttribute("filename");
            int line_number = XmlUtils::ReadLong(msg, "lineno");
            
            wxString localFile = ::MapRemoteFileToLocalFile(filename);
            CL_DEBUG("Mapping remote file: %s => %s", filename, localFile);
            wxFileName fnFilename( localFile );
            if(fnFilename.Exists()) {
                // Notify about control
                XDebugEvent focusEvent(wxEVT_XDEBUG_IDE_GOT_CONTROL);
                focusEvent.SetFileName( fnFilename.GetFullPath() );
                focusEvent.SetLineNumber( line_number - 1 ); // scintilla is counting from 0
                EventNotifier::Get()->AddPendingEvent( focusEvent );
            } else {
                wxString message;
                message << _("Failed to map remote file: ") << filename << "\n"
                        << _("Check your project settings->Debug to define folder mapping");
                ::wxMessageBox(message, "CodeLite", wxICON_WARNING|wxOK|wxCENTER);
            }
        }
    }
}
