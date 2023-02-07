#include "StdioTransport.hpp"

dap::StdioTransport::StdioTransport() {}

dap::StdioTransport::~StdioTransport() {}

bool dap::StdioTransport::Read(wxString& buffer, int msTimeout)
{
    wxUnusedVar(buffer);
    wxUnusedVar(msTimeout);
    return false;
}

size_t dap::StdioTransport::Send(const wxString& buffer)
{
    wxUnusedVar(buffer);
    return false;
}
