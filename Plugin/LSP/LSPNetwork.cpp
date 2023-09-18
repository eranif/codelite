#include "LSPNetwork.h"

wxDEFINE_EVENT(wxEVT_LSP_NET_DATA_READY, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LSP_NET_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LSP_NET_CONNECTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LSP_NET_LOGMSG, clCommandEvent);

LSPNetwork::LSPNetwork() {}

LSPNetwork::~LSPNetwork() {}

wxString& LSPNetwork::wrap_with_quotes(wxString& str)
{
    if(!str.empty() && str.Contains(" ") && !str.StartsWith("\"") && !str.EndsWith("\"")) {
        str.Prepend("\"").Append("\"");
    }
    return str;
}

wxString LSPNetwork::BuildCommand(const wxArrayString& args)
{
    if(args.empty()) {
        return "";
    }
    // Build command line from the array
    wxString command;
    command << args[0];

    wrap_with_quotes(command);
    for(size_t i = 1; i < args.size(); ++i) {
        wxString argument = args[i];
        wrap_with_quotes(argument);
        command << " " << argument;
    }
    return command;
}

void LSPNetwork::LogMessage(int sev, const wxString& msg)
{
    clCommandEvent log_error_event{ wxEVT_LSP_NET_LOGMSG };
    log_error_event.SetString(msg);
    log_error_event.SetInt(sev);
    AddPendingEvent(log_error_event);
}

void LSPNetwork::NotifyError(const wxString& reason)
{
    clCommandEvent error_event{ wxEVT_LSP_NET_ERROR };
    error_event.SetString(reason);
    AddPendingEvent(error_event);
}
