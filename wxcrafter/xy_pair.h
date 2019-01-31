#ifndef XYPAIR_H
#define XYPAIR_H

#include <wx/string.h>

class XYPair
{
protected:
    wxString m_string;
    int m_x;
    int m_y;

public:
    XYPair(const wxString& str, int defaultx = -1, int defaulty = -1);
    XYPair(int x, int y);
    virtual ~XYPair();

    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    wxString ToString(bool withBraces = false) const;

    bool operator>(const XYPair& xy) const;
    bool operator==(const XYPair& xy) const;
    bool operator!=(const XYPair& xy) const;
};

#endif // XYPAIR_H
