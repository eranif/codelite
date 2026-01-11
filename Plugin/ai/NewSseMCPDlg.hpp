#ifndef NEWSSEMCPDLG_HPP
#define NEWSSEMCPDLG_HPP

#include "UI.hpp"
#include "ai/LLMManager.hpp"
#include "codelite_exports.h"

#include <map>
#include <string>

class WXDLLIMPEXP_SDK NewSseMCPDlg : public NewSseMCPDlgBase
{
public:
    NewSseMCPDlg(wxWindow* parent);
    ~NewSseMCPDlg() override;

    llm::SSEMcp GetData() const;

protected:
    void OnOkUI(wxUpdateUIEvent& event) override;
    void OnHeaders(wxCommandEvent& event) override;

private:
    std::map<std::string, std::string> ProcessKeyValueEntry(wxTextCtrl* text_ctrl, const wxString& prefix);

    std::string m_name;
    std::map<std::string, std::string> m_headers;
    std::string m_baseUrl;
    std::string m_endpoint;
    std::string m_authtoken;
};
#endif // NEWSSEMCPDLG_HPP
