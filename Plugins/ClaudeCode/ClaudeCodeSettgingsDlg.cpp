#include "ClaudeCodeSettgingsDlg.hpp"

#include "cl_config.h"

ClaudeCodeSettgingsDlg::ClaudeCodeSettgingsDlg(wxWindow* parent)
    : ClaudeCodeSettgingsBaseDlg(parent)
{
    m_textCtrlClaudeExec->SetValue(clConfig::Get().Read(kClaudeCodeExecutable, wxString{}));
}

ClaudeCodeSettgingsDlg::~ClaudeCodeSettgingsDlg() {}
