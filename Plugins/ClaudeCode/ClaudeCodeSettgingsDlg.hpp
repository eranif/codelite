#pragma once

#include "ClaudeCodeUI.hpp"

inline const wxString kClaudeCodeExecutable = "claude-code/executable";

class ClaudeCodeSettgingsDlg : public ClaudeCodeSettgingsBaseDlg
{
public:
    ClaudeCodeSettgingsDlg(wxWindow* parent);
    ~ClaudeCodeSettgingsDlg() override;

    inline wxString GetClaudeCode() const
    {
        auto exe = m_textCtrlClaudeExec->GetValue();
        exe.Trim().Trim(false);
        return exe;
    }
};
