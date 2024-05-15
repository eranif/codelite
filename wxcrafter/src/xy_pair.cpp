#include "xy_pair.h"

#include "wxgui_helpers.h"

XYPair::XYPair(wxString str, int defaultx, int defaulty)
    : m_x(defaultx)
    , m_y(defaulty)
{
    if (str.StartsWith(wxT("(")))
        str.Remove(0, 1);

    if (str.EndsWith(wxT(")")))
        str.RemoveLast();

    wxString strx = str.BeforeFirst(wxT(','));
    wxString stry = str.AfterFirst(wxT(','));

    strx.Trim().Trim(false);
    stry.Trim().Trim(false);

    m_x = wxCrafter::ToNumber(strx, defaultx);
    m_y = wxCrafter::ToNumber(stry, defaulty);
}

XYPair::XYPair(int x, int y)
    : m_x(x)
    , m_y(y)
{
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
