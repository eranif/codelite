#ifndef CLCELLVALUE_H
#define CLCELLVALUE_H

#include "codelite_exports.h"
#include <vector>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clCellValue
{
    wxString m_text;
    int m_bitmapIndex = wxNOT_FOUND;
    int m_bitmapSelectedIndex = wxNOT_FOUND;
    wxFont m_font;
    wxColour m_textColour;
    wxColour m_bgColour;
    bool m_isNull = true;
public:
    clCellValue();
    clCellValue(const wxString& text, int bmpIndex = wxNOT_FOUND, int bmpOpenIndex = wxNOT_FOUND);
    virtual ~clCellValue();
    bool IsOk() const { return !m_isNull; }

    void SetText(const wxString& text) { this->m_text = text; }
    int GetBitmapIndex() const { return m_bitmapIndex; }
    int GetBitmapSelectedIndex() const { return m_bitmapSelectedIndex; }
    const wxString& GetText() const { return m_text; }
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetBitmapIndex(int bitmapIndex) { this->m_bitmapIndex = bitmapIndex; }
    void SetBitmapSelectedIndex(int bitmapSelectedIndex) { this->m_bitmapSelectedIndex = bitmapSelectedIndex; }
    void SetFont(const wxFont& font) { this->m_font = font; }
    void SetTextColour(const wxColour& textColour) { this->m_textColour = textColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxFont& GetFont() const { return m_font; }
    const wxColour& GetTextColour() const { return m_textColour; }

    typedef std::vector<clCellValue> Vect_t;
};

#endif // CLCELLVALUE_H
