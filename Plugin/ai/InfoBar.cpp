#include "ai/InfoBar.hpp"

#include "file_logger.h"

void InfoBar::ShowMessage(const wxString& msg, int flags)
{
    m_pendingMessages.push_back({msg, flags});
    if (m_pendingMessages.size() > 1) {
        return;
    }
    wxInfoBar::ShowMessage(msg, flags);
}

void InfoBar::Dismiss()
{
    m_pendingMessages.erase(m_pendingMessages.begin());
    if (m_pendingMessages.empty()) {
        wxInfoBar::Dismiss();
        return;
    }
    auto p = m_pendingMessages.front();
    m_pendingMessages.erase(m_pendingMessages.begin());
    wxInfoBar::ShowMessage(p.first, p.second);
}
