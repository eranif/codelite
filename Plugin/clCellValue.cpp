#include "clCellValue.h"

clCellValue::clCellValue() {}

clCellValue::clCellValue(const wxString& text, int bmpIndex, int bmpOpenIndex)
    : m_stringValue(text)
    , m_type(kTypeString)
    , m_bitmapIndex(bmpIndex)
    , m_bitmapSelectedIndex(bmpOpenIndex)
{
}

clCellValue::clCellValue(bool bValue, const wxString& label, int bmpIndex, int bmpOpenIndex)
    : m_boolValue(bValue)
    , m_stringValue(label)
    , m_type(kTypeBool)
    , m_bitmapIndex(bmpIndex)
    , m_bitmapSelectedIndex(bmpOpenIndex)
{
}

clCellValue::~clCellValue() {}

clCellValue::clCellValue(const char* ptext, int bmpIndex, int bmpOpenIndex)
    : clCellValue(wxString(ptext), bmpIndex, bmpOpenIndex)
{
}

const wxString& clCellValue::GetValueString() const
{
    if(IsChoice()) {
        if(m_selection == wxNOT_FOUND) {
            static wxString EmptyString;
            return EmptyString;
        } else {
            return m_options.Item(m_selection);
        }
    } else {
        return m_stringValue;
    }
}

void clCellValue::SetValue(const wxString& text)
{
     if(IsChoice()) {
         if(m_options.Index(text) == wxNOT_FOUND) {
             m_options.Add(text);
             m_selection = (int)(m_options.size() - 1);
         }
     } else {
         this->m_stringValue = text;
     }
}
