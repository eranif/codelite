#include "clInfoBar.h"
#include <algorithm>

clInfoBar::clInfoBar(wxWindow* parent, wxWindowID winid)
    : wxInfoBar(parent, winid)
{
}

clInfoBar::~clInfoBar() {}

void clInfoBar::Clear()
{
    std::for_each(m_buttons.begin(), m_buttons.end(),
                  [&](const std::pair<wxWindowID, wxString>& btn) { RemoveButton(btn.first); });
}

void clInfoBar::DisplayMessage(const wxString& message, int flags,
                               const std::vector<std::pair<wxWindowID, wxString> >& buttons)
{
    Clear();
    m_buttons = buttons;
    if(m_buttons.empty()) { m_buttons.push_back({ wxID_OK, "" }); }
    std::for_each(m_buttons.begin(), m_buttons.end(),
                  [&](const std::pair<wxWindowID, wxString>& btn) { AddButton(btn.first, btn.second); });
    ShowMessage(message, flags);
}
