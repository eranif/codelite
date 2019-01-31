#ifndef WXCEVENT_H
#define WXCEVENT_H

#include "wxcLib/wxcEnums.h"
#include <cl_command_event.h>
#include <vector>
#include <wx/filename.h>

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------

class wxcNetworkEvent : public clCommandEvent
{
public:
    typedef std::vector<wxFileName> FileNameVect_t;

protected:
    wxCrafter::eCommandType m_commandType;
    wxcNetworkEvent::FileNameVect_t m_files;
    int m_formId;

public:
    wxcNetworkEvent(wxEventType eventType, int winid = 0);
    virtual ~wxcNetworkEvent();
    wxcNetworkEvent(const wxcNetworkEvent& src);
    wxcNetworkEvent& operator=(const wxcNetworkEvent& src);

    virtual wxEvent* Clone() const { return new wxcNetworkEvent(*this); }

    void SetCommandType(const wxCrafter::eCommandType& commandType) { this->m_commandType = commandType; }
    const wxCrafter::eCommandType& GetCommandType() const { return m_commandType; }
    void SetFiles(const wxcNetworkEvent::FileNameVect_t& files) { this->m_files = files; }
    const wxcNetworkEvent::FileNameVect_t& GetFiles() const { return m_files; }
    void SetFormId(int formId) { this->m_formId = formId; }
    int GetFormId() const { return m_formId; }
};

typedef void (wxEvtHandler::*wxcEventFunction)(wxcNetworkEvent&);
#define wxcEventHandler(func) wxEVENT_HANDLER_CAST(wxcEventFunction, func)

// -----------------------------------------------------------------------------------
// Declare the event types
// -----------------------------------------------------------------------------------
wxDECLARE_EVENT(wxEVT_NETWORK_COMMAND_SHOW_DESIGNER, wxcNetworkEvent);
wxDECLARE_EVENT(wxEVT_NETWORK_COMMAND_EXIT, wxcNetworkEvent);
wxDECLARE_EVENT(wxEVT_NETWORK_COMMAND_LOAD_FILE, wxcNetworkEvent);
wxDECLARE_EVENT(wxEVT_NETWORK_COMMAND_CONN_ESTASBLISHED, wxcNetworkEvent);
wxDECLARE_EVENT(wxEVT_NETWORK_COMMAND_CONN_GEN_CODE, wxcNetworkEvent);
wxDECLARE_EVENT(wxEVT_NETWORK_COMMAND_NEW_FORM, wxcNetworkEvent);

#endif // WXCEVENT_H
