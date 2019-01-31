#include "wxcEvent.h"
#include <wx/filename.h>

// -----------------------------------------------------------------------------------
// Events definitions
// -----------------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_NETWORK_COMMAND_SHOW_DESIGNER, wxcNetworkEvent);
wxDEFINE_EVENT(wxEVT_NETWORK_COMMAND_EXIT, wxcNetworkEvent);
wxDEFINE_EVENT(wxEVT_NETWORK_COMMAND_LOAD_FILE, wxcNetworkEvent);
wxDEFINE_EVENT(wxEVT_NETWORK_COMMAND_CONN_ESTASBLISHED, wxcNetworkEvent);
wxDEFINE_EVENT(wxEVT_NETWORK_COMMAND_CONN_GEN_CODE, wxcNetworkEvent);
wxDEFINE_EVENT(wxEVT_NETWORK_COMMAND_NEW_FORM, wxcNetworkEvent);

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
wxcNetworkEvent::wxcNetworkEvent(wxEventType eventType, int winid)
    : clCommandEvent(eventType, winid)
    , m_commandType(wxCrafter::kCommandTypeInvalid)
    , m_formId(wxNOT_FOUND)
{
}

wxcNetworkEvent::wxcNetworkEvent(const wxcNetworkEvent& src) { *this = src; }

wxcNetworkEvent::~wxcNetworkEvent() {}

wxcNetworkEvent& wxcNetworkEvent::operator=(const wxcNetworkEvent& src)
{
    clCommandEvent::operator=(src);
    m_commandType = src.m_commandType;
    m_files = src.m_files;
    m_formId = src.m_formId;
    return *this;
}
