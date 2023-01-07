#include "xy_pair.h"
#include "wxgui_helpers.h"

XYPair::XYPair(const wxString& str, int defaultx, int defaulty)
    : m_string(str)
    , m_x(defaultx)
    , m_y(defaulty)
{
    m_string.Trim().Trim(false);
    if(m_string.StartsWith(wxT("("))) m_string.Remove(0, 1);

    if(m_string.EndsWith(wxT(")"))) m_string.RemoveLast();

    wxString strx = m_string.BeforeFirst(wxT(','));
    wxString stry = m_string.AfterFirst(wxT(','));

    strx.Trim().Trim(false);
    stry.Trim().Trim(false);

    m_x = wxCrafter::ToNumber(strx, defaultx);
    m_y = wxCrafter::ToNumber(stry, defaulty);
}

XYPair::XYPair(int x, int y)
    : m_x(x)
    , m_y(y)
{
    m_string = ToString(false);
}

XYPair::~XYPair() {}

wxString XYPair::ToString(bool withBraces) const
{
    wxString str;
    str << m_x << wxT(",") << m_y;
    if(withBraces) { str.Prepend(wxT("(")).Append(wxT(")")); }
    return str;
}

bool XYPair::operator>(const XYPair& xy) const
{
    if(GetX() == xy.GetX()) {
        return GetY() > xy.GetY();

    } else {
        return GetX() > xy.GetX();
    }
}

bool XYPair::operator==(const XYPair& xy) const { return GetX() == xy.GetX() && GetY() == xy.GetY(); }

bool XYPair::operator!=(const XYPair& xy) const { return !(*this == xy); }
