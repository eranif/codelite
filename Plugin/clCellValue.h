#ifndef CLCELLVALUE_H
#define CLCELLVALUE_H

#include "codelite_exports.h"
#include "drawingutils.h"

#include <vector>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/string.h>

enum class eCellButtonType {
    BT_NONE = -1,
    BT_DROPDOWN_ARROW = 0,
    BT_ELLIPSIS = 1,
};

class WXDLLIMPEXP_SDK clCellValue
{
public:
    enum eType {
        kTypeNull = 0,
        kTypeString = 1,
        kTypeBool = 2,
        kTypeButton = 3,
        kTypeColour = 4,
        kTypeOnlyButton = 5,
    };
    typedef std::vector<clCellValue> Vect_t;

protected:
    bool m_boolValue = false;
    wxString m_stringValue;
    eType m_type = kTypeNull;
    eCellButtonType m_button_type = eCellButtonType::BT_NONE;
    int m_bitmapIndex = wxNOT_FOUND;
    int m_bitmapSelectedIndex = wxNOT_FOUND;
    wxFont m_font;
    wxColour m_textColour;
    wxColour m_bgColour;
    wxRect m_checkboxRect;
    wxRect m_buttonRect;
    wxColour m_colourValue;
    wxString m_buttonUnicodeSymbol;
    eButtonState m_button_state = eButtonState::kNormal;

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
    void SetValue(const wxColour& c) { this->m_colourValue = c; }
    const wxString& GetValueString() const;
    bool GetValueBool() const { return m_boolValue; }
    const wxColour& GetValueColour() const { return m_colourValue; }

    void SetButtonState(const eButtonState& button_state) { this->m_button_state = button_state; }
    const eButtonState& GetButtonState() const { return m_button_state; }

    /**
     * @brief this cell has button in addition to ...
     */
    bool HasButton() const { return m_type == kTypeButton; }

    /**
     * @brief the entire cell is a button
     */
    bool IsButton() const { return m_type == kTypeOnlyButton; }

    bool IsColour() const { return m_type == kTypeColour; }
    eCellButtonType GetButtonType() const { return m_button_type; }
    void SetButtonType(eCellButtonType type, const wxString& unicode_symbol)
    {
        m_button_type = type;
        m_buttonUnicodeSymbol = unicode_symbol;
    }

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
    void SetButtonRect(const wxRect& dropDownRect) { this->m_buttonRect = dropDownRect; }
    const wxRect& GetButtonRect() const { return m_buttonRect; }
    const wxString& GetButtonUnicodeSymbol() const { return m_buttonUnicodeSymbol; }
};

#endif // CLCELLVALUE_H
