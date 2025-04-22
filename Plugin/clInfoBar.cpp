#include "clInfoBar.h"

#include <algorithm>

clInfoBar::clInfoBar(wxWindow* parent, wxWindowID winid)
    : wxInfoBar(parent, winid)
{
}

clInfoBar::~clInfoBar() {}

void clInfoBar::Clear()
{
    for (const auto& [btnId, _] : m_buttons) {
        RemoveButton(btnId);
    }
}

void clInfoBar::DisplayMessage(const wxString& message, int flags,
                               const std::vector<std::pair<wxWindowID, wxString>>& buttons)
{
    Clear();
    m_buttons = buttons;
    if(m_buttons.empty()) {
        m_buttons.push_back({ wxID_OK, "" });
    }

    for(const auto& [id, label] : m_buttons) {
        AddButton(id, label);
    }
    ShowMessage(message, flags);
}
