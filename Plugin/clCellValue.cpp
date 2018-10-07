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
