#include "clCellValue.h"

clCellValue::clCellValue() {}

clCellValue::clCellValue(const wxString& text, int bmpIndex, int bmpOpenIndex)
    : m_text(text)
    , m_bitmapIndex(bmpIndex)
    , m_bitmapSelectedIndex(bmpOpenIndex)
    , m_isNull(false)
{
}

clCellValue::~clCellValue() {}
