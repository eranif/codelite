#ifndef NEWLOCALMCPDLG_HPP
#define NEWLOCALMCPDLG_HPP

#include "UI.hpp"
#include "ai/LLMManager.hpp"
#include "codelite_exports.h"

#include <map>
#include <string>
#include <vector>

class WXDLLIMPEXP_SDK NewLocalMCPDlg : public NewLocalMCPDlgBase
{
public:
    NewLocalMCPDlg(wxWindow* parent);
    ~NewLocalMCPDlg() override;
    llm::LocalMcp GetData() const;

protected:
    void OnOkUI(wxUpdateUIEvent& event) override;
    void OnEnvVariables(wxCommandEvent& event) override;
    std::map<std::string, std::string> ProcessKeyValueEntry(wxTextCtrl* text_ctrl, const wxString& prefix);

private:
    std::map<std::string, std::string> m_envVariables; // Environment variables
};
#endif // NEWLOCALMCPDLG_HPP
