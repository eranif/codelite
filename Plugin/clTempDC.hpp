#ifndef CLTEMPDC_HPP
#define CLTEMPDC_HPP

#include "clTabRenderer.h"

#include <wx/bitmap.h>

class wxGCDC;
class wxMemoryDC;

class WXDLLIMPEXP_SDK clTempDC
{
    wxBitmap m_tmpBmp;
    wxMemoryDC* m_memDC = nullptr;
    wxGCDC* m_gcdc = nullptr;

public:
    clTempDC();
    ~clTempDC();

    wxDC& GetDC() { return *m_gcdc; }
    const wxDC& GetDC() const { return *m_gcdc; }
};

#endif // CLTEMPDC_HPP
