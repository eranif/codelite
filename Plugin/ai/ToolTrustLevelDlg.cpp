#include "ToolTrustLevelDlg.hpp"

#include "event_notifier.h"

#include <algorithm>

ToolTrustLevelDlg::ToolTrustLevelDlg(wxWindow* parent,
                                     const std::vector<std::pair<wxString, wxString>>& options,
                                     size_t initial)
    : ToolTrustLevelDlgBase(parent == nullptr ? EventNotifier::Get()->TopFrame() : parent)
{
    m_data = options;
    for (const auto& [display_string, value] : options) {
        wxVector<wxVariant> cols;
        cols.push_back(display_string);
        m_dvListCtrlOptions->AppendItem(cols);
    }

    if (initial < m_dvListCtrlOptions->GetItemCount()) {
        m_dvListCtrlOptions->SelectRow(initial);
    }

    CenterOnParent();
}

std::optional<wxString> ToolTrustLevelDlg::GetValue() const
{
    auto item = m_dvListCtrlOptions->GetSelection();
    if (!item.IsOk()) {
        return std::nullopt;
    }

    wxString display_string = m_dvListCtrlOptions->GetTextValue(m_dvListCtrlOptions->ItemToRow(item), 0);
    auto iter =
        std::ranges::find_if(m_data, [&display_string](const auto& p) -> bool { return p.first == display_string; });

    if (iter == m_data.end()) {
        return std::nullopt;
    }

    return iter->second;
}
