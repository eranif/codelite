#include "LSPNetwork.h"

wxDEFINE_EVENT(wxEVT_LSP_NET_DATA_READY, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LSP_NET_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_LSP_NET_CONNECTED, clCommandEvent);

LSPNetwork::LSPNetwork() {}

LSPNetwork::~LSPNetwork() {}

wxString& LSPNetwork::wrap_with_quotes(wxString& str)
{
    if(str.Contains(" ")) {
        // since we are going to convert this into a string, we need to escape all backslashes
        str.Replace("\\", "\\\\");
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