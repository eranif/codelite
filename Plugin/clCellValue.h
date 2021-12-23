#ifndef CLCELLVALUE_H
#define CLCELLVALUE_H

#include "codelite_exports.h"

#include <vector>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clCellValue
{
public:
    enum eType {
        kTypeNull = 0,
        kTypeString = 1,
        kTypeBool = 2,
        kTypeChoice = 3,
    };

protected:
    bool m_boolValue = false;
    wxString m_stringValue;
    eType m_type = kTypeNull;
    int m_bitmapIndex = wxNOT_FOUND;
    int m_bitmapSelectedIndex = wxNOT_FOUND;
    wxFont m_font;
    wxColour m_textColour;
    wxColour m_bgColour;
    wxRect m_checkboxRect;
    wxRect m_dropDownRect;

public:
    clCellValue();
    clCellValue(const wxString& text, int bmpIndex = wxNOT_FOUND, int bmpOpenIndex = wxNOT_FOUND);
    clCellValue(const char* ptext, int bmpIndex = wxNOT_FOUND, int bmpOpenIndex = wxNOT_FOUND);
    clCellValue(bool bValue, const wxString& label, int bmpIndex = wxNOT_FOUND, int bmpOpenIndex = wxNOT_FOUND);
    virtual ~clCellValue();
    bool IsOk() const { return m_type != kTypeNull; }
    bool IsString() const { return m_type == kTypeString; }
    bool IsBool() const { return m_type == kTypeBool; }
    void SetType(eType type) { m_type = type; }
    void SetValue(const wxString& text);
    void SetValue(bool b) { this->m_boolValue = b; }
    const wxString& GetValueString() const;
    bool GetValueBool() const { return m_boolValue; }
    bool IsChoice() const { return m_type == kTypeChoice; }

    int GetBitmapIndex() const { return m_bitmapIndex; }
    int GetBitmapSelectedIndex() const { return m_bitmapSelectedIndex; }
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetBitmapIndex(int bitmapIndex) { this->m_bitmapIndex = bitmapIndex; }
    void SetBitmapSelectedIndex(int bitmapSelectedIndex) { this->m_bitmapSelectedIndex = bitmapSelectedIndex; }
    void SetFont(const wxFont& font) { this->m_font = font; }
    void SetTextColour(const wxColour& textColour) { this->m_textColour = textColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxFont& GetFont() const { return m_font; }
    const wxColour& GetTextColour() const { return m_textColour; }
    void SetCheckboxRect(const wxRect& checkboxRect) { this->m_checkboxRect = checkboxRect; }
    const wxRect& GetCheckboxRect() const { return m_checkboxRect; }
    void SetDropDownRect(const wxRect& dropDownRect) { this->m_dropDownRect = dropDownRect; }
    const wxRect& GetDropDownRect() const { return m_dropDownRect; }
    typedef std::vector<clCellValue> Vect_t;
};

#endif // CLCELLVALUE_H
