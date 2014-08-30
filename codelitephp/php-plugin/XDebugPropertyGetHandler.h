#ifndef XDEBUGPROPERTYGETHANDLER_H
#define XDEBUGPROPERTYGETHANDLER_H

#include <wx/string.h>
#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugPropertyGetHandler : public XDebugCommandHandler
{
    wxString m_property;
    
public:
    XDebugPropertyGetHandler(XDebugManager* mgr, int transcationId, const wxString &property);
    virtual ~XDebugPropertyGetHandler();

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGPROPERTYGETHANDLER_H
