#pragma once

#include "UI.hpp"
#include "codelite_exports.h"

#include <optional>
#include <vector>

class WXDLLIMPEXP_SDK ToolTrustLevelDlg : public ToolTrustLevelDlgBase
{
public:
    ToolTrustLevelDlg(wxWindow* parent, const std::vector<std::pair<wxString, wxString>>& options, size_t initial = 0);
    ~ToolTrustLevelDlg() override = default;

    bool IsChecked() const { return m_checkBox->IsChecked(); }
    void Check(bool b) { m_checkBox->SetValue(b); }
    std::optional<wxString> GetValue() const;
    void SetMessage(const wxString& message) { m_staticTextMessage->SetLabel(message); }

private:
    std::vector<std::pair<wxString, wxString>> m_data;
};
