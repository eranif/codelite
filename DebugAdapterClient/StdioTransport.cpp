#include "StdioTransport.hpp"

dap::StdioTransport::StdioTransport() {}

dap::StdioTransport::~StdioTransport() {}

bool dap::StdioTransport::Read(std::string& buffer, int msTimeout)
{
    wxUnusedVar(buffer);
    wxUnusedVar(msTimeout);
    return false;
}

size_t dap::StdioTransport::Send(const std::string& buffer)
{
    wxUnusedVar(buffer);
    return false;
}
