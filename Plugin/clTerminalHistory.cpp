#include "clTerminalHistory.h"

#include <algorithm>

static wxString sEmptyStr;

clTerminalHistory::clTerminalHistory()
    : m_where(-1)
{
}

clTerminalHistory::~clTerminalHistory() {}

void clTerminalHistory::DoReset() { m_where = -1; }

void clTerminalHistory::Add(const wxString& command)
{
    std::vector<wxString>::iterator iter =
        std::find_if(m_history.begin(), m_history.end(), [&](const wxString& str) { return str == command; });
    if(iter != m_history.end()) {
        m_history.erase(iter);
    }
    m_history.insert(m_history.begin(), command);
    DoReset();
}

const wxString& clTerminalHistory::ArrowUp()
{
    if((m_where + 1) < (int)m_history.size()) {
        ++m_where;
        return m_history[m_where];
    }
    return sEmptyStr;
}

const wxString& clTerminalHistory::ArrowDown()
{
    if(m_where > 0 && m_where < (int)m_history.size()) {
        m_where--;
        return m_history[m_where];
    }
    return sEmptyStr;
}

wxArrayString clTerminalHistory::GetItems() const
{
    wxArrayString items;
    std::for_each(m_history.begin(), m_history.end(), [&](const wxString& item) { items.Add(item); });
    return items;
}

void clTerminalHistory::SetItems(const wxArrayString& items)
{
    m_history.clear();
    for(size_t i = 0; i < items.GetCount(); ++i) {
        Add(items.Item(i));
    }
}
