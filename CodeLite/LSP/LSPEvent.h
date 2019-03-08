#ifndef LSPEVENT_H
#define LSPEVENT_H

#include "cl_command_event.h"
#include "LSP/basic_types.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL LSPEvent : public clCommandEvent
{
    LSP::Location m_location;
    wxString m_serverName;

public:
    LSPEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    LSPEvent(const LSPEvent& src);
    LSPEvent& operator=(const LSPEvent& other);
    virtual ~LSPEvent();
    LSPEvent& SetLocation(const LSP::Location& location)
    {
        this->m_location = location;
        return *this;
    }
    const LSP::Location& GetLocation() const { return m_location; }
    wxEvent* Clone() const { return new LSPEvent(*this); }
    LSPEvent& SetServerName(const wxString& serverName)
    {
        this->m_serverName = serverName;
        return *this;
    }
    const wxString& GetServerName() const { return m_serverName; }
};

typedef void (wxEvtHandler::*LSPEventFunction)(LSPEvent&);
#define LSPEventHandler(func) wxEVENT_HANDLER_CAST(LSPEventFunction, func)


wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_DEFINITION, LSPEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_LSP_INITIALIZED, LSPEvent);

#endif // LSPEVENT_H
