#include "clConnectionString.h"
#include "file_logger.h"

clConnectionString::clConnectionString(const wxString& connectionString)
    : m_port(wxNOT_FOUND)
    , m_isOK(false)
{
    DoParse(connectionString);
}

clConnectionString::~clConnectionString() {}

void clConnectionString::DoParse(const wxString& connectionString)
{
    m_isOK = false; // default
    // get the protocol part
    clDEBUG() << "Parsing connection string:" << connectionString << clEndl;
    wxString protocol = connectionString.BeforeFirst(':');
    if(protocol == "tcp") {
        m_protocol = kTcp;
    } else if(protocol == "unix") {
#ifdef __WXMSW__
        clWARNING() << "unix protocol is not suppported on Windows" << clEndl;
        return;
#endif
        m_protocol = kUnixLocalSocket;
    } else {
        clWARNING() << "Invalid protocol in connection string:" << connectionString << clEndl;
        return;
    }

    wxString address = connectionString.AfterFirst(':');
    address = address.Mid(2); // Skip the "//"
    if(m_protocol == kUnixLocalSocket) {
        // The rest is the file path
        m_path = address;
        m_isOK = !m_path.IsEmpty();
    } else {
        // we now expect host[:port]
        m_host = address.BeforeFirst(':');
        wxString port = address.AfterFirst(':');
        if(!port.IsEmpty()) {
            port.ToCLong(&m_port);
        }
        m_isOK = !m_host.IsEmpty() && (m_port != wxNOT_FOUND);
    }
}
