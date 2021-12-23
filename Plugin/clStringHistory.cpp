#include "clStringHistory.h"

clStringHistory::clStringHistory()
    : m_index(0)
{
}

clStringHistory::~clStringHistory() {}

bool clStringHistory::Current(wxString& str)
{
    if(m_strings.IsEmpty() || (m_index < 0 || m_index >= (int)m_strings.GetCount()))
        return false;

    str = m_strings.Item(m_index);
    return true;
}

bool clStringHistory::Next(wxString& str)
{
    if(CanNext()) {
        ++m_index;
        str = m_strings.Item(m_index);
        return true;
    }
    return false;
}

bool clStringHistory::Previous(wxString& str)
{
    if(CanPrev()) {
        --m_index;
        str = m_strings.Item(m_index);
        return true;
    }
    return false;
}

bool clStringHistory::CanNext()
{
    if(m_strings.IsEmpty())
        return false;
    return (m_index + 1) < (int)m_strings.GetCount();
}

bool clStringHistory::CanPrev()
{
    if(m_strings.IsEmpty())
        return false;
    return (m_index - 1) >= 0;
}
