#include "clStringView.hpp"

#include <wx/wxcrt.h>

clStringView::clStringView() {}

clStringView::~clStringView() {}

wxString clStringView::MakeString() const
{
    if(!m_pdata) {
        return wxString();
    } else {
        return wxString(m_pdata, m_length);
    }
}

clStringView::clStringView(const wxChar* p, size_t len)
{
    m_pdata = p;
    m_length = len;
}

bool clStringView::Advance(size_t count)
{
    // sanity: if the requested count is more than the current length
    // clear the string
    if(count > m_length) {
        return false;
    }

    m_pdata += count;
    m_length -= count;
    return true;
}

int clStringView::Find(const wxString& what) const
{
    if(empty()) {
        return wxNOT_FOUND;
    }
    const wxChar* match = ::wxStrstr(m_pdata, what);
    if(match == nullptr) {
        return wxNOT_FOUND;
    }
    return match - m_pdata;
}

void clStringView::Reset(const wxString& buffer)
{
    m_pdata = buffer.c_str();
    m_length = buffer.length();
}
