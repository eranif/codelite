#ifndef NEWMCPSERVERDLG_HPP
#define NEWMCPSERVERDLG_HPP

#include "UI.hpp"
#include "clPropertiesPage.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK NewMCPServerDlg : public NewMCPServerDlgBase
{
public:
    NewMCPServerDlg(wxWindow* parent);
    ~NewMCPServerDlg() override;

    const std::map<std::string, std::string>& GetHeaders() const { return m_headers; }
    const std::string& GetBaseUrl() const { return m_baseUrl; }
    const std::string& GetEndpoint() const { return m_endpoint; }
    const std::string& GetAuthtoken() const { return m_authtoken; }
    const std::vector<std::string>& GetCommand() const { return m_command; }
    const std::map<std::string, std::string>& GetEnvVariables() const { return m_envVariables; }
    bool IsStdioServer() const;
    bool IsSSEServer() const;

protected:
    void OnServerTypeChanged(wxCommandEvent& event) override;
    void OnActioButton(clPropertiesPageEvent& event);

private:
    void Initialise();
    void UpdateUIForStdio();
    void UpdateUIForSSE();
    std::map<std::string, std::string> ProcessKeyValueEntry(clPropertiesPageEvent& event, const wxString& prefix);

    clPropertiesPage* m_view{nullptr};
    std::map<std::string, std::string> m_headers;      // Used by SSE server.
    std::string m_baseUrl;                             // Used by SSE server.
    std::string m_endpoint;                            // Used by SSE server.
    std::string m_authtoken;                           // Used by SSE server.
    std::vector<std::string> m_command;                // Used by STDIO server.
    std::map<std::string, std::string> m_envVariables; // Used by STDIO server.
};

#endif // NEWMCPSERVERDLG_HPP
